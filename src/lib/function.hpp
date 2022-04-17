#pragma once

#include "plugin.hpp"

#include "./nodes/videonode.hpp"

class Function : public Napi::ObjectWrap<Function> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    Function(const Napi::CallbackInfo &);
    ~Function();

    Function *SetInstance(Core *core, Plugin *plugin, VSPluginFunction *vsfunction);
    static Napi::Object CreateInstance(Core *core, Plugin *plugin, VSPluginFunction *vsfunction);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    VSPluginFunction *vsfunction{nullptr};
    Plugin *plugin{nullptr};
    Core *core{nullptr};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);
    Napi::Value GetPlugin(const Napi::CallbackInfo &);

    Napi::Value GetName(const Napi::CallbackInfo &);
    Napi::Value GetSignature(const Napi::CallbackInfo &);
    Napi::Value GetReturnType(const Napi::CallbackInfo &);
    Napi::Value Call(const Napi::CallbackInfo &);

    Napi::Value IsVideoInjectable(const Napi::CallbackInfo &);
    Napi::Value IsAudioInjectable(const Napi::CallbackInfo &);

    const char *getName();
};
