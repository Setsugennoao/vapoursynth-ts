#pragma once

#include "core.hpp"

class VideoFormat : public Napi::ObjectWrap<VideoFormat> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoFormat(const Napi::CallbackInfo &);
    ~VideoFormat();

    VideoFormat *SetInstance(Core *core, const VSVideoFormat *vsvformat);
    static Napi::Object CreateInstance(Core *core, const VSVideoFormat *vsvformat);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    const VSVideoFormat *vsvformat{nullptr};
    Core *core{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value ToObject(const Napi::CallbackInfo &);
    Napi::Value Replace(const Napi::CallbackInfo &);
};
