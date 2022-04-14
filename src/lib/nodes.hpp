#pragma once

#include <sstream>

#include "core.hpp"

class VideoFormat : public Napi::ObjectWrap<VideoFormat> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoFormat(const Napi::CallbackInfo &);
    ~VideoFormat();

    void SetVideoFormat(Core *core, VSVideoFormat *vsnode);

    Napi::Value ToObject(const Napi::CallbackInfo &);
    Napi::Value Replace(const Napi::CallbackInfo &);

    static Napi::FunctionReference *constructor;

    VSVideoFormat *vsformat{nullptr};

  private:
    Core *core{nullptr};
};

class RawNode : public Napi::ObjectWrap<RawNode> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    RawNode(const Napi::CallbackInfo &);
    ~RawNode();

    void SetRawNode(Core *core, VSNode *vsnode);

    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetFrames(const Napi::CallbackInfo &);

    static Napi::FunctionReference *constructor;

    Core *core{nullptr};
    VSNode *vsnode{nullptr};

  private:
};

class VideoNode : public Napi::ObjectWrap<VideoNode> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    VideoNode(const Napi::CallbackInfo &);
    ~VideoNode();

    void SetNode(Core *core, VSNode *vsnode);

    static Napi::Object CreateNode(Core *core, VSNode *vsnode);
    int getFrameSize();

    static Napi::FunctionReference *constructor;

    RawNode *node{nullptr};
    const VSVideoInfo *vsvideoinfo{nullptr};

  private:

    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetWidth(const Napi::CallbackInfo &);
    Napi::Value GetHeight(const Napi::CallbackInfo &);
    Napi::Value GetFormat(const Napi::CallbackInfo &);
    Napi::Value GetNumFrames(const Napi::CallbackInfo &);
    Napi::Value GetFrameSize(const Napi::CallbackInfo &);
    Napi::Value GetFps(const Napi::CallbackInfo &);

    Napi::Value GetFrame(const Napi::CallbackInfo &);
    Napi::Value GetFrames(const Napi::CallbackInfo &);
};
