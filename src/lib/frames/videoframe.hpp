#pragma once

#include <sstream>

#include "../core.hpp"

#include "rawframe.hpp"


class VideoFrame : public Napi::ObjectWrap<VideoFrame> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoFrame(const Napi::CallbackInfo &);
    ~VideoFrame();

    void SetFrame(Core *core, const VSFrame *constvsframe, VSFrame *vsframe);

    static Napi::Object CreateVideoFrame(Core *core, VSFrame *constvsframe);
    static Napi::Object CreateVideoFrame(Core *core, const VSFrame *constvsframe);

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    RawFrame *frame{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetWidth(const Napi::CallbackInfo &);
    Napi::Value GetHeight(const Napi::CallbackInfo &);
    Napi::Value GetFormat(const Napi::CallbackInfo &);
};
