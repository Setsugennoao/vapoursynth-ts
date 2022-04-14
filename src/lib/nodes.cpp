#include "nodes.hpp"

/**********************************************************************************************************************/

class VideoFrameWorker : public Napi::AsyncWorker {
  public:
    VideoFrameWorker(VideoNode *vnode, int32_t frameNumber, Napi::Buffer<char> &buffer, bool ownBuffer):
        Napi::AsyncWorker(vnode->Env()), ownBuffer(ownBuffer), vnode(vnode), vsapi(vnode->node->core->vsapi),
        frameNumber(frameNumber), bufferHandle(Napi::Persistent(buffer)), outBuffer(buffer.Data()),
        deferred(Napi::Promise::Deferred::New(vnode->Env())) {}
    VideoFrameWorker(VideoNode *vnode, int32_t frameNumber, Napi::Buffer<char> &buffer, bool ownBuffer, Napi::Function &callback):
        Napi::AsyncWorker(callback), ownBuffer(ownBuffer), vnode(vnode), vsapi(vnode->node->core->vsapi),
        frameNumber(frameNumber), bufferHandle(Napi::Persistent(buffer)), outBuffer(buffer.Data()),
        deferred(Napi::Promise::Deferred::New(vnode->Env())) {}

    ~VideoFrameWorker() {
        if (ownBuffer) bufferHandle.Unref();
    }

    void Execute() {
        char errMsg[1024];
        const VSFrame *frame = vnode->node->core->vsapi->getFrame(frameNumber, vnode->node->vsnode, errMsg, sizeof(errMsg));

        if (!frame) {
            std::ostringstream ss;
            ss << "Encountered error getting frame " << frameNumber << ": " << errMsg;
            SetError(ss.str());
            return;
        }

        for (int p = 0; p < vnode->vsvideoinfo->format.numPlanes; p++) {
            ptrdiff_t stride = vsapi->getStride(frame, p);
            const uint8_t *readPtr = vsapi->getReadPtr(frame, p);
            int rowSize = vsapi->getFrameWidth(frame, p) * vnode->vsvideoinfo->format.bytesPerSample;
            int height = vsapi->getFrameHeight(frame, p);

            for (int y = 0; y < height; y++) {
                memmove(outBuffer, readPtr, rowSize);
                outBuffer += rowSize;
                readPtr += stride;
            }
        }

        vnode->node->core->vsapi->freeFrame(frame);
    }

    void OnOK() {
        deferred.Resolve(bufferHandle.Value());
    }

    void OnError(Napi::Error const &error) {
        deferred.Reject(error.Value());
    }

    Napi::Promise GetPromise() {
        return deferred.Promise();
    }

  protected:
    std::vector<napi_value> GetResult(Napi::Env env) {
        return {
            env.Null(),
            Napi::Number::New(env, frameNumber),
            bufferHandle.Value()
        };
    }

  private:
    VideoNode *vnode;
    const VSAPI *vsapi;
    int32_t frameNumber;
    char *outBuffer;
    bool ownBuffer;
    Napi::Promise::Deferred deferred;
    Napi::Reference<Napi::Buffer<char>> bufferHandle;
};

/**********************************************************************************************************************/

Napi::Object RawNode::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RawNode", {
        InstanceAccessor<&RawNode::GetCore>("core"),
        InstanceMethod<&RawNode::GetFrames>("frames")
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("RawNode", func);

    return exports;
}

RawNode::RawNode(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RawNode>(info) {}

void RawNode::SetRawNode(Core *core, VSNode *vsnode) {
    this->core = core;
    this->vsnode = vsnode;
}

Napi::FunctionReference *RawNode::constructor;

RawNode::~RawNode() {
    if (core && vsnode) {
        core->vsapi->freeNode(vsnode);
    }
}

Napi::Value RawNode::GetFrames(const Napi::CallbackInfo &info) {
    return Napi::Value::From(info.Env(), "frames"); // TODO
}

Napi::Value RawNode::GetCore(const Napi::CallbackInfo &info) {
    return core->GetCoreObject();
}

/**********************************************************************************************************************/

Napi::Object VideoNode::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "VideoNode", {
        InstanceAccessor<&VideoNode::GetCore>("core"),
        InstanceAccessor<&VideoNode::GetFps>("fps"),
        InstanceAccessor<&VideoNode::GetWidth>("width"),
        InstanceAccessor<&VideoNode::GetHeight>("height"),
        InstanceAccessor<&VideoNode::GetFormat>("format"),
        InstanceAccessor<&VideoNode::GetNumFrames>("numFrames"),
        InstanceAccessor<&VideoNode::GetFrameSize>("frameSize"),
        InstanceMethod<&VideoNode::GetFrame>("getFrame"),
        InstanceMethod<&VideoNode::GetFrames>("frames"),
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("VideoNode", func);

    return exports;
}

VideoNode::VideoNode(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VideoNode>(info) {}

void VideoNode::SetNode(Core *core, VSNode *vsnode) {
    if (node) {
        node->~RawNode();
    }
    Napi::Object rawNodeObject = RawNode::constructor->New({});
    node = RawNode::Unwrap(rawNodeObject);
    node->SetRawNode(core, vsnode);

    vsvideoinfo = core->vsapi->getVideoInfo(node->vsnode);
}

Napi::Object VideoNode::CreateNode(Core *core, VSNode *vsnode) {
    Napi::Object videoNodeObject = constructor->New({});
    VideoNode *videoNode = VideoNode::Unwrap(videoNodeObject);
    videoNode->SetNode(core, vsnode);
    Napi::Function proxy = core->proxyFunctions->Get("VideoNode").As<Napi::Function>();
    return proxy.Call({videoNodeObject}).As<Napi::Object>().Get("__self").As<Napi::Object>();
}

Napi::FunctionReference *VideoNode::constructor;

VideoNode::~VideoNode() {
    node->~RawNode();
}

int VideoNode::getFrameSize() {
    if (!vsvideoinfo) {
        return 0;
    }

    int frame_size = (vsvideoinfo->width * vsvideoinfo->format.bytesPerSample) >> vsvideoinfo->format.subSamplingW;
    if (frame_size) {
        frame_size *= vsvideoinfo->height;
        frame_size >>= vsvideoinfo->format.subSamplingH;
        frame_size *= 2;
    }
    frame_size += vsvideoinfo->width * vsvideoinfo->format.bytesPerSample * vsvideoinfo->height;

    return frame_size;
}

Napi::Value VideoNode::GetWidth(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Value::From(env, vsvideoinfo->width);
}

Napi::Value VideoNode::GetHeight(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Value::From(env, vsvideoinfo->height);
}

Napi::Value VideoNode::GetFormat(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (vsvideoinfo->format.colorFamily == cfUndefined) {
        return env.Null();
    }

    return Napi::Value::From(env, vsvideoinfo->numFrames);
}

Napi::Value VideoNode::GetNumFrames(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Value::From(env, vsvideoinfo->numFrames);
}

Napi::Value VideoNode::GetFrameSize(const Napi::CallbackInfo &info) {
    return Napi::Number::New(info.Env(), getFrameSize());
}

Napi::Value VideoNode::GetFps(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    Napi::Object fpsObject = Napi::Object::New(env);

    fpsObject.Set("numerator", vsvideoinfo->fpsNum);
    fpsObject.Set("denominator", vsvideoinfo->fpsDen);

    return Napi::Value::From(env, fpsObject);
}

Napi::Value VideoNode::GetFrame(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    bool ownBuffer{info.Length() > 1 && info[1].IsBuffer()};
    bool isAsync{!info[info.Length() - 1].IsFunction()};

    int32_t frameNumber = info[0].As<Napi::Number>().Int32Value();

    Napi::Buffer<char> buffer = info.Length() > 1 ? info[1].As<Napi::Buffer<char>>() : Napi::Buffer<char>::New(env, getFrameSize());

    if (isAsync) {
        VideoFrameWorker *worker = new VideoFrameWorker(this, frameNumber, buffer, ownBuffer);

        worker->Queue();

        Napi::Promise promise = worker->GetPromise();

        return promise;
    } else {
        Napi::Function callback = info[ownBuffer ? 2 : 1].As<Napi::Function>();

        VideoFrameWorker *worker = new VideoFrameWorker(this, frameNumber, buffer, ownBuffer, callback);

        worker->Queue();

        return env.Null();
    }
}

Napi::Value VideoNode::GetFrames(const Napi::CallbackInfo &info) {
    return node->GetFrames(info);
}

Napi::Value VideoNode::GetCore(const Napi::CallbackInfo &info) {
    return node->GetCore(info);
}
