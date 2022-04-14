#pragma once

#include "core.hpp"

class VideoFormat : public Napi::ObjectWrap<VideoFormat> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoFormat(const Napi::CallbackInfo &);
    ~VideoFormat();

    void SetVideoFormat(Core *core, const VSVideoFormat *vsvformat);

    static Napi::Object CreateVideoFormat(Core *core, const VSVideoFormat *vsvformat);

    Napi::Value ToObject(const Napi::CallbackInfo &);
    Napi::Value Replace(const Napi::CallbackInfo &);

    static Napi::FunctionReference *constructor;

    const VSVideoFormat *vsvformat{nullptr};

  private:
    Core *core{nullptr};
};
