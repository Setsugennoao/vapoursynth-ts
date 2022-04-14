#pragma once

#include <sstream>

#include "../core.hpp"

#include "rawframe.hpp"


class VideoFrame : public Napi::ObjectWrap<VideoFrame> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoFrame(const Napi::CallbackInfo &);
    ~VideoFrame();

    void SetFrame(Core *core, VSFrame *vsframe, bool writable);

    static Napi::Object CreateVideoFrame(Core *core, VSFrame *vsframe, bool writable);

    static Napi::FunctionReference *constructor;

    RawFrame *frame{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetWidth(const Napi::CallbackInfo &);
    Napi::Value GetHeight(const Napi::CallbackInfo &);
    Napi::Value GetFormat(const Napi::CallbackInfo &);
};
