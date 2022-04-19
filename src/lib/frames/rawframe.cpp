#include "rawframe.hpp"

Napi::Object RawFrame::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RawFrame", {
        InstanceAccessor<&RawFrame::GetCore>("core"),
        InstanceAccessor<&RawFrame::GetProps>("props"),
        InstanceAccessor<&RawFrame::GetFlags>("flags"),
        InstanceAccessor<&RawFrame::GetIsReadOnly>("readonly"),
        InstanceAccessor<&RawFrame::GetIsClosed>("closed"),
        InstanceMethod<&RawFrame::CloseFrame>("close"),
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("RawFrame", func);

    return exports;
}

RawFrame::RawFrame(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RawFrame>(info) {}

Napi::Object RawFrame::GetProxyObject() {
    Napi::Function proxyFunction = core->proxyFunctions->Get(constvsframe ? "RawFrame" : "RawFrameEditable").As<Napi::Function>();
    return proxyFunction.Call({this->Value()}).As<Napi::Object>();
}

RawFrame *RawFrame::SetInstance(Core *core, VSFrame *vsframe) {
    this->core = core;

    if (this->vsframe) core->vsapi->freeFrame(this->vsframe);
    if (this->constvsframe) core->vsapi->freeFrame(this->constvsframe);

    this->vsframe = vsframe;
    this->constvsframe = vsframe;
    this->flags = -1;

    return this;
}

RawFrame *RawFrame::SetInstance(Core *core, const VSFrame *vsframe) {
    this->core = core;

    if (this->vsframe) core->vsapi->freeFrame(this->vsframe);
    if (this->constvsframe) core->vsapi->freeFrame(this->constvsframe);

    this->vsframe = nullptr;
    this->constvsframe = vsframe;
    this->flags = 0;

    return this;
}

RawFrame *RawFrame::CreateInstance(Core *core, const VSFrame *constvsframe, VSFrame *vsframe) {
    return RawFrame::Unwrap(constructor->New({}))->SetInstance(core, constvsframe ? constvsframe : vsframe);
}

Napi::FunctionReference *RawFrame::constructor;

RawFrame::~RawFrame() {
    if (constvsframe) {
        core->vsapi->freeFrame(constvsframe);
    }
    if (vsframe) {
        core->vsapi->freeFrame(vsframe);
    }
    constvsframe = nullptr;
    vsframe = nullptr;
}

void RawFrame::ensureOpen(const Napi::CallbackInfo &info) {
    if (constvsframe == nullptr) {
        Napi::Error::New(info.Env(), "The Frame has already been released!").ThrowAsJavaScriptException();
    }
}

Napi::Value RawFrame::GetCore(const Napi::CallbackInfo &info) {
    return core->GetProxyObject();
}

Napi::Value RawFrame::GetProps(const Napi::CallbackInfo &info) {
    ensureOpen(info);
    // return FrameProps::CreateFrameProps(vsframe);
    return Napi::Object::Object({});
}

Napi::Value RawFrame::GetFlags(const Napi::CallbackInfo &info) {
    return Napi::Number::From(info.Env(), flags);
}

Napi::Value RawFrame::GetIsClosed(const Napi::CallbackInfo &info) {
    return Napi::Boolean::From(info.Env(), constvsframe == nullptr);
}

Napi::Value RawFrame::GetIsReadOnly(const Napi::CallbackInfo &info) {
    return Napi::Boolean::From(info.Env(), !(bool)(flags & 1));
}

void RawFrame::CloseFrame(const Napi::CallbackInfo &info) {
    this->~RawFrame();
}
