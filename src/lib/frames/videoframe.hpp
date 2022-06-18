#pragma once

#include <sstream>

#include "../core.hpp"

#include "rawframe.hpp"


class VideoFrame : public Napi::ObjectWrap<VideoFrame> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoFrame(const Napi::CallbackInfo &);
    ~VideoFrame();

    VideoFrame *SetInstance(Core *core, VSFrame *vsframe);
    VideoFrame *SetInstance(Core *core, const VSFrame *constvsframe);
    static VideoFrame *CreateInstance(Core *core, VSFrame *vsframe);
    static VideoFrame *CreateInstance(Core *core, const VSFrame *constvsframe);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    RawFrame *rawframe{nullptr};
    Core *core{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetWidth(const Napi::CallbackInfo &);
    Napi::Value GetHeight(const Napi::CallbackInfo &);
    Napi::Value GetFormat(const Napi::CallbackInfo &);

    Napi::Value GetProps(const Napi::CallbackInfo &);
    Napi::Value GetFlags(const Napi::CallbackInfo &);

    void CloseFrame(const Napi::CallbackInfo &);
    Napi::Value GetIsClosed(const Napi::CallbackInfo &);
    Napi::Value GetIsReadOnly(const Napi::CallbackInfo &);
};
