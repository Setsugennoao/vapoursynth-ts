#pragma once

#include "core.hpp"

#include "./nodes/rawnode.hpp"

class Plugin : public Napi::ObjectWrap<Plugin> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    Plugin(const Napi::CallbackInfo &);
    ~Plugin();

    void SetPlugin(Core *core, VSPlugin *vsplugin, RawNode *injectedNode);
    Napi::Object GetPluginObject();

    static Napi::Object CreatePlugin(Core *core, VSPlugin *vsplugin, RawNode *injectedNode);

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    RawNode *injectedNode{nullptr};
    VSPlugin *vsplugin{nullptr};

  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetNamespace(const Napi::CallbackInfo &);
    Napi::Value GetDescription(const Napi::CallbackInfo &);
    Napi::Value GetFunction(const Napi::CallbackInfo &);
    Napi::Value GetAllFunctions(const Napi::CallbackInfo &);

    Core *core{nullptr};
};
