#include "../formats.hpp"

#include "videoframe.hpp"

/**********************************************************************************************************************/

Napi::Object VideoFrame::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "VideoFrame", {
        InstanceAccessor<&VideoFrame::GetCore>("core"),
        InstanceAccessor<&VideoFrame::GetWidth>("width"),
        InstanceAccessor<&VideoFrame::GetHeight>("height"),
        InstanceAccessor<&VideoFrame::GetFormat>("format"),
        InstanceAccessor<&VideoFrame::GetProps>("props"),
        InstanceAccessor<&VideoFrame::GetFlags>("flags"),
        InstanceAccessor<&VideoFrame::GetIsReadOnly>("readonly"),
        InstanceAccessor<&VideoFrame::GetIsClosed>("closed"),
        InstanceMethod<&VideoFrame::CloseFrame>("close"),
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

VideoFrame *VideoFrame::SetInstance(Core *core, VSFrame *vsframe) {
    this->core = core;

    this->rawframe = RawFrame::CreateInstance(core, nullptr, vsframe);

    return this;
}

VideoFrame *VideoFrame::SetInstance(Core *core, const VSFrame *constvsframe) {
    this->core = core;

    if (rawframe) {
        rawframe->~RawFrame();
    }

    this->rawframe = RawFrame::CreateInstance(core, constvsframe, nullptr);

    return this;
}

VideoFrame *VideoFrame::CreateInstance(Core *core, VSFrame *vsframe) {
    return VideoFrame::Unwrap(constructor->New({}))->SetInstance(core, vsframe);
}

VideoFrame *VideoFrame::CreateInstance(Core *core, const VSFrame *constvsframe) {
    return VideoFrame::Unwrap(constructor->New({}))->SetInstance(core, constvsframe);
}

Napi::FunctionReference *VideoFrame::constructor;

VideoFrame::~VideoFrame() {
    rawframe->~RawFrame();
}

Napi::Value VideoFrame::GetWidth(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Number::From(
        env,
        rawframe->core->vsapi->getFrameWidth(
            rawframe->flags & 1 ? rawframe->vsframe : rawframe->constvsframe, 0
        )
    );
}

Napi::Value VideoFrame::GetHeight(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();


    return Napi::Number::From(
        env,
        rawframe->core->vsapi->getFrameHeight(
            rawframe->flags & 1 ? rawframe->vsframe : rawframe->constvsframe, 0
        )
    );
}

Napi::Value VideoFrame::GetFormat(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return VideoFormat::CreateInstance(
        rawframe->core,
        rawframe->core->vsapi->getVideoFrameFormat(
            rawframe->flags & 1 ? rawframe->vsframe : rawframe->constvsframe
        )
    );
}

Napi::Value VideoFrame::GetCore(const Napi::CallbackInfo &info) {
    return core->GetProxyObject();
}

Napi::Value VideoFrame::GetProps(const Napi::CallbackInfo &info) {
    return rawframe->GetProps(info);
}

Napi::Value VideoFrame::GetFlags(const Napi::CallbackInfo &info) {
    return rawframe->GetFlags(info);
}

Napi::Value VideoFrame::GetIsClosed(const Napi::CallbackInfo &info) {
    return rawframe->GetIsClosed(info);
}

Napi::Value VideoFrame::GetIsReadOnly(const Napi::CallbackInfo &info) {
    return rawframe->GetIsReadOnly(info);
}

void VideoFrame::CloseFrame(const Napi::CallbackInfo &info) {
    this->~VideoFrame();
}
