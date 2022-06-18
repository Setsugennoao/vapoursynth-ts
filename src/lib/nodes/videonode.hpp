#pragma once

#include <sstream>

#include "../core.hpp"

#include "rawnode.hpp"


class VideoNode : public Napi::ObjectWrap<VideoNode> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoNode(const Napi::CallbackInfo &);
    ~VideoNode();

    VideoNode *SetInstance(Core *core, VSNode *vsnode);
    static Napi::Object CreateInstance(Core *core, VSNode *vsnode);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    int getFrameSize();
    static int getFrameSize(int width, int height, const VSVideoFormat *videoformat);

    const VSVideoInfo *vsvideoinfo{nullptr};
    RawNode *rawnode{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetWidth(const Napi::CallbackInfo &);
    Napi::Value GetHeight(const Napi::CallbackInfo &);
    Napi::Value GetFormat(const Napi::CallbackInfo &);
    Napi::Value GetNumFrames(const Napi::CallbackInfo &);
    Napi::Value GetFrameSize(const Napi::CallbackInfo &);
    Napi::Value GetFps(const Napi::CallbackInfo &);

    void SetOutput(const Napi::CallbackInfo &);
    Napi::Value GetFrame(const Napi::CallbackInfo &);
    Napi::Value GetFrameAsync(const Napi::CallbackInfo &);
};
