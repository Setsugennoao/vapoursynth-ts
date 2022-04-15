#pragma once

#include "core.hpp"

class Plugin : public Napi::ObjectWrap<Plugin> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    Plugin(const Napi::CallbackInfo &);
    ~Plugin();

    void SetPlugin(Core *core, VSPlugin *vsplugin);
    Napi::Object GetPluginObject();

    static Napi::Object CreatePlugin(Core *core, VSPlugin *vsplugin);

    static Napi::FunctionReference *constructor;

    VSPlugin *vsplugin{nullptr};

  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);

    Napi::Value GetNamespace(const Napi::CallbackInfo &);
    Napi::Value GetDescription(const Napi::CallbackInfo &);
    Napi::Value GetFunction(const Napi::CallbackInfo &);
    Napi::Value GetAllFunctions(const Napi::CallbackInfo &);

    Core *core{nullptr};
};
