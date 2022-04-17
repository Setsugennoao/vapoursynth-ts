#pragma once

#include <sstream>

#include "../core.hpp"

class RawNode : public Napi::ObjectWrap<RawNode> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    RawNode(const Napi::CallbackInfo &);
    ~RawNode();

    RawNode *SetInstance(Core *core, VSNode *vsnode);
    static RawNode *CreateInstance(Core *core, VSNode *vsnode);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }


    Napi::Value GetFrames(const Napi::CallbackInfo &);

    VSNode *vsnode{nullptr};
    Core *core{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);
};
