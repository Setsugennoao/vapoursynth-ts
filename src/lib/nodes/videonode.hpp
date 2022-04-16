#pragma once

#include <sstream>

#include "../core.hpp"

#include "rawnode.hpp"


class VideoNode : public Napi::ObjectWrap<VideoNode> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoNode(const Napi::CallbackInfo &);
    ~VideoNode();

    void SetNode(Core *core, VSNode *vsnode);

    static Napi::Object CreateNode(Core *core, VSNode *vsnode);
    int getFrameSize();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    RawNode *node{nullptr};
    const VSVideoInfo *vsvideoinfo{nullptr};

  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

   void SetOutput(const Napi::CallbackInfo &);

    Napi::Value GetWidth(const Napi::CallbackInfo &);
    Napi::Value GetHeight(const Napi::CallbackInfo &);
    Napi::Value GetFormat(const Napi::CallbackInfo &);
    Napi::Value GetNumFrames(const Napi::CallbackInfo &);
    Napi::Value GetFrameSize(const Napi::CallbackInfo &);
    Napi::Value GetFps(const Napi::CallbackInfo &);

    Napi::Value GetFrame(const Napi::CallbackInfo &);
    Napi::Value GetFrames(const Napi::CallbackInfo &);
};
