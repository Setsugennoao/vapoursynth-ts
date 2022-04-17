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

Napi::Object VideoFrame::GetProxyObject() {
    return rawframe->core->proxyFunctions->Get(
        rawframe->constvsframe ? "VideoFrame" : "VideoFrameEditable"
    ).As<Napi::Function>().Call({this->Value()}).As<Napi::Object>();
}

VideoFrame *VideoFrame::SetInstance(Core *core, const VSFrame *constvsframe, VSFrame *vsframe) {
    if (rawframe) {
        rawframe->~RawFrame();
    }

    rawframe = RawFrame::CreateInstance(core, constvsframe, vsframe);

    return this;
}

Napi::Object VideoFrame::CreateInstance(Core *core, const VSFrame *vsframe) {
    return VideoFrame::Unwrap(constructor->New({}))->SetInstance(core, vsframe, nullptr)->GetProxyObject();
}

Napi::Object VideoFrame::CreateInstance(Core *core, VSFrame *vsframe) {
    return VideoFrame::Unwrap(constructor->New({}))->SetInstance(core, nullptr, vsframe)->GetProxyObject();
}

Napi::FunctionReference *VideoFrame::constructor;

VideoFrame::~VideoFrame() {
    rawframe->~RawFrame();
}

Napi::Value VideoFrame::GetWidth(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Value::From(env, rawframe->core->vsapi->getFrameWidth(rawframe->vsframe, 0));
}

Napi::Value VideoFrame::GetHeight(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();


    return Napi::Value::From(env, rawframe->core->vsapi->getFrameHeight(rawframe->vsframe, 0));
}

Napi::Value VideoFrame::GetFormat(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return VideoFormat::CreateInstance(rawframe->core, rawframe->core->vsapi->getVideoFrameFormat(rawframe->vsframe));
}

Napi::Value VideoFrame::GetCore(const Napi::CallbackInfo &info) {
    return rawframe->core->GetProxyObject();
}
