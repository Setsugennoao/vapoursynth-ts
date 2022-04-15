#include "../formats.hpp"

#include "videoframe.hpp"

/**********************************************************************************************************************/

Napi::Object VideoFrame::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "VideoFrame", {
        InstanceAccessor<&VideoFrame::GetCore>("core"),
        InstanceAccessor<&VideoFrame::GetWidth>("width"),
        InstanceAccessor<&VideoFrame::GetHeight>("height"),
        InstanceAccessor<&VideoFrame::GetFormat>("format"),
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("VideoFrame", func);

    return exports;
}

VideoFrame::VideoFrame(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VideoFrame>(info) {}

void VideoFrame::SetFrame(Core *core, const VSFrame *constvsframe, VSFrame *vsframe) {
    if (frame) {
        frame->~RawFrame();
    }
    Napi::Object rawNodeObject = RawFrame::constructor->New({});
    frame = RawFrame::Unwrap(rawNodeObject);
    if (constvsframe) {
        frame->SetRawNode(core, constvsframe);
    } else {
        frame->SetRawNode(core, vsframe);
    }
}

Napi::Object VideoFrame::CreateVideoFrame(Core *core, const VSFrame *vsframe) {
    Napi::Object videoFrameObject = constructor->New({});
    VideoFrame *videoFrame = VideoFrame::Unwrap(videoFrameObject);
    videoFrame->SetFrame(core, vsframe, nullptr);
    Napi::Function proxy = core->proxyFunctions->Get("VideoFrame").As<Napi::Function>();
    return proxy.Call({videoFrameObject}).As<Napi::Object>();
}

Napi::Object VideoFrame::CreateVideoFrame(Core *core, VSFrame *vsframe) {
    Napi::Object videoFrameObject = constructor->New({});
    VideoFrame *videoFrame = VideoFrame::Unwrap(videoFrameObject);
    videoFrame->SetFrame(core, nullptr, vsframe);
    Napi::Function proxy = core->proxyFunctions->Get("VideoFrameEditable").As<Napi::Function>();
    return proxy.Call({videoFrameObject}).As<Napi::Object>();
}

Napi::FunctionReference *VideoFrame::constructor;

VideoFrame::~VideoFrame() {
    frame->~RawFrame();
}

Napi::Value VideoFrame::GetWidth(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Value::From(env, frame->core->vsapi->getFrameWidth(frame->vsframe, 0));
}

Napi::Value VideoFrame::GetHeight(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();


    return Napi::Value::From(env, frame->core->vsapi->getFrameHeight(frame->vsframe, 0));
}

Napi::Value VideoFrame::GetFormat(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return VideoFormat::CreateVideoFormat(frame->core, frame->core->vsapi->getVideoFrameFormat(frame->vsframe));
}

Napi::Value VideoFrame::GetCore(const Napi::CallbackInfo &info) {
    return frame->GetCore(info);
}
