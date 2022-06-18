#include "../formats.hpp"

#include "videonode.hpp"

/**********************************************************************************************************************/

class VideoFrameWorker : public Napi::AsyncWorker {
  public:
    VideoFrameWorker(VideoNode *vnode, int32_t frameNumber, Napi::Buffer<char> &buffer, bool ownBuffer):
        Napi::AsyncWorker(vnode->Env()), ownBuffer(ownBuffer), vnode(vnode), vsapi(vnode->rawnode->core->vsapi),
        frameNumber(frameNumber), bufferHandle(Napi::Persistent(buffer)), outBuffer(buffer.Data()),
        deferred(Napi::Promise::Deferred::New(vnode->Env())) {}
    VideoFrameWorker(VideoNode *vnode, int32_t frameNumber, Napi::Buffer<char> &buffer, bool ownBuffer, Napi::Function &callback):
        Napi::AsyncWorker(callback), ownBuffer(ownBuffer), vnode(vnode), vsapi(vnode->rawnode->core->vsapi),
        frameNumber(frameNumber), bufferHandle(Napi::Persistent(buffer)), outBuffer(buffer.Data()),
        deferred(Napi::Promise::Deferred::New(vnode->Env())) {}

    ~VideoFrameWorker() {
        if (ownBuffer) bufferHandle.Unref();
    }

    void Execute() {
        char errMsg[1024];
        const VSFrame *frame = vnode->rawnode->core->vsapi->getFrame(frameNumber, vnode->rawnode->vsnode, errMsg, sizeof(errMsg));

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

        vnode->rawnode->core->vsapi->freeFrame(frame);
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

Napi::Object VideoNode::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "VideoNode", {
        InstanceAccessor<&VideoNode::GetCore>("core"),
        InstanceAccessor<&VideoNode::GetFps>("fps"),
        InstanceAccessor<&VideoNode::GetWidth>("width"),
        InstanceAccessor<&VideoNode::GetHeight>("height"),
        InstanceAccessor<&VideoNode::GetFormat>("format"),
        InstanceAccessor<&VideoNode::GetNumFrames>("numFrames"),
        InstanceAccessor<&VideoNode::GetFrameSize>("frameSize"),
        InstanceMethod<&VideoNode::SetOutput>("setOutput"),
        InstanceMethod<&VideoNode::GetFrame>("getFrame"),
        InstanceMethod<&VideoNode::GetFrameAsync>("getFrameAsync")
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("VideoNode", func);

    return exports;
}

VideoNode::VideoNode(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VideoNode>(info) {}

Napi::Object VideoNode::GetProxyObject() {
    return rawnode->core->proxyFunctions->Get("VideoNode").As<Napi::Function>().Call({this->Value()}).As<Napi::Object>();
}

VideoNode *VideoNode::SetInstance(Core *core, VSNode *vsnode) {
    if (rawnode) {
        rawnode->~RawNode();
    }

    rawnode = RawNode::CreateInstance(core, vsnode);

    vsvideoinfo = core->vsapi->getVideoInfo(rawnode->vsnode);

    return this;
}

Napi::Object VideoNode::CreateInstance(Core *core, VSNode *vsnode) {
    return VideoNode::Unwrap(constructor->New({}))->SetInstance(core, vsnode)->GetProxyObject();
}

Napi::FunctionReference *VideoNode::constructor;

VideoNode::~VideoNode() {
    rawnode->~RawNode();
}

int VideoNode::getFrameSize() {
    return getFrameSize(vsvideoinfo->width, vsvideoinfo->height, &vsvideoinfo->format);
}

int VideoNode::getFrameSize(int width, int height, const VSVideoFormat *format) {
    if (!width || !height || !format) {
        return 0;
    }

    int frame_size = (width * format->bytesPerSample) >> format->subSamplingW;
    if (frame_size) {
        frame_size *= height;
        frame_size >>= format->subSamplingH;
        frame_size *= 2;
    }
    frame_size += width * format->bytesPerSample * height;

    return frame_size;
}

void VideoNode::SetOutput(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::Error::New(Env(), "Index has to be a number!").ThrowAsJavaScriptException();
        return;
    }

    if (info.Length() > 3) {
        Napi::Error::New(Env(), "Invalid number of arguments!").ThrowAsJavaScriptException();
        return;
    }

    int index = info[0].As<Napi::Number>().Int32Value();

    Napi::Object outputObject = Napi::Object::New(env);

    outputObject.Set("clip", this->GetProxyObject());
    if (info.Length() >= 2 && info[1].ToBoolean().Value()) {
        VideoNode *alpha = VideoNode::Unwrap(info[1].As<Napi::Object>());

        if ((vsvideoinfo->width != alpha->vsvideoinfo->width) || (vsvideoinfo->height != alpha->vsvideoinfo->height)) {
            Napi::Error::New(Env(), "Alpha clip dimensions must match the main video!").ThrowAsJavaScriptException();
            return;
        }
        if (vsvideoinfo->numFrames != alpha->vsvideoinfo->numFrames) {
            Napi::Error::New(Env(), "Alpha clip length must match the main video!").ThrowAsJavaScriptException();
            return;
        }

        if ((vsvideoinfo->format.colorFamily != cfUndefined) && (alpha->vsvideoinfo->format.colorFamily != cfUndefined)) {
            if (
                (alpha->vsvideoinfo->format.colorFamily != cfGray) ||
                (alpha->vsvideoinfo->format.sampleType != vsvideoinfo->format.sampleType) ||
                (alpha->vsvideoinfo->format.bitsPerSample != vsvideoinfo->format.bitsPerSample)
            ) {
                Napi::Error::New(Env(), "Alpha clip format must match the main video!").ThrowAsJavaScriptException();
                return;
            }
        } else if ((vsvideoinfo->format.colorFamily != cfUndefined) || (alpha->vsvideoinfo->format.colorFamily != cfUndefined)) {
            Napi::Error::New(Env(), "Format must be either known or unknown for both alpha and main clip!").ThrowAsJavaScriptException();
            return;
        }

        outputObject.Set("alpha", alpha->GetProxyObject());
    } else {
        outputObject.Set("alpha", env.Null());
    }

    outputObject.Set("altOutput", info.Length() == 3 ? info[2].As<Napi::Number>() : env.Null());

    rawnode->core->setOutput(index, outputObject);
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

    return VideoFormat::CreateInstance(rawnode->core, &vsvideoinfo->format);
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

    int32_t frameNumber = info[0].As<Napi::Number>().Int32Value();

    Napi::Buffer<char> buffer = info.Length() > 1 ? info[1].As<Napi::Buffer<char>>() : Napi::Buffer<char>::New(env, getFrameSize());

    Napi::Function callback = info[ownBuffer ? 2 : 1].As<Napi::Function>();

    VideoFrameWorker *worker = new VideoFrameWorker(this, frameNumber, buffer, ownBuffer, callback);

    worker->Queue();

    return env.Null();
}

Napi::Value VideoNode::GetFrameAsync(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    bool ownBuffer{info.Length() > 1 && info[1].IsBuffer()};

    int32_t frameNumber = info[0].As<Napi::Number>().Int32Value();

    Napi::Buffer<char> buffer = info.Length() > 1 ? info[1].As<Napi::Buffer<char>>() : Napi::Buffer<char>::New(env, getFrameSize());

    VideoFrameWorker *worker = new VideoFrameWorker(this, frameNumber, buffer, ownBuffer);

    worker->Queue();

    Napi::Promise promise = worker->GetPromise();

    return promise;
}

Napi::Value VideoNode::GetCore(const Napi::CallbackInfo &info) {
    return rawnode->core->GetProxyObject();
}
