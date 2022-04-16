#pragma once

#include <sstream>

#include "../core.hpp"

class RawNode : public Napi::ObjectWrap<RawNode> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    RawNode(const Napi::CallbackInfo &);
    ~RawNode();

    void SetRawNode(Core *core, VSNode *vsnode);

    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetFrames(const Napi::CallbackInfo &);

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    Core *core{nullptr};
    VSNode *vsnode{nullptr};
};
