#include "formats.hpp"

Napi::Object VideoFormat::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "VideoFormat", {
        InstanceMethod("replace", &VideoFormat::Replace)
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("VideoFormat", func);

    return exports;
}

VideoFormat::VideoFormat(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VideoFormat>(info) {}

Napi::Object VideoFormat::GetProxyObject() {
    return core->proxyFunctions->Get("VideoFormat").As<Napi::Function>().Call({this->Value()}).As<Napi::Object>();
}

VideoFormat *VideoFormat::SetInstance(Core *core, const VSVideoFormat *vsvformat) {
    this->core = core;
    this->vsvformat = vsvformat;

    return this;
}

Napi::Object VideoFormat::CreateInstance(Core *core, const VSVideoFormat *vsvformat) {
    return VideoFormat::Unwrap(constructor->New({}))->SetInstance(core, vsvformat)->GetProxyObject();
}

Napi::FunctionReference *VideoFormat::constructor;

VideoFormat::~VideoFormat() {}

Napi::Value VideoFormat::ToObject(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    char nameBuffer[32];

    core->vsapi->getVideoFormatName(vsvformat, nameBuffer);

    Napi::Object videoFormatObj = Napi::Object::New(env);

    const VSVideoFormat f = *vsvformat;

    videoFormatObj.Set("id", Napi::Value::From(
        env, core->vsapi->queryVideoFormatID(
            f.colorFamily, f.sampleType, f.bitsPerSample, f.subSamplingW, f.subSamplingH, core->vscore
        )
    ));
    videoFormatObj.Set("name", Napi::String::From(env, nameBuffer));
    videoFormatObj.Set("colorFamily", Napi::Number::From(env, f.colorFamily));
    videoFormatObj.Set("sampleType", Napi::Number::From(env, f.sampleType));
    videoFormatObj.Set("bitsPerSample", Napi::Number::From(env, f.bitsPerSample));
    videoFormatObj.Set("bytesPerSample", Napi::Number::From(env, f.bytesPerSample));
    videoFormatObj.Set("subsamplingW", Napi::Number::From(env, f.subSamplingW));
    videoFormatObj.Set("subsamplingH", Napi::Number::From(env, f.subSamplingH));
    videoFormatObj.Set("numPlanes", Napi::Number::From(env, f.numPlanes));

    return Napi::Value::From(env, videoFormatObj);
}

Napi::Value VideoFormat::Replace(const Napi::CallbackInfo &info) {
    return Napi::Value::From(info.Env(), "cià");
}
