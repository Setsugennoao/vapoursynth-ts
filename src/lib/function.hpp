#pragma once

#include "nodes.hpp"
#include "plugin.hpp"

class Function : public Napi::ObjectWrap<Function> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    Function(const Napi::CallbackInfo &);
    ~Function();

    void SetFunction(Core *core, Plugin *plugin, VSPluginFunction *vsfunction);

    const char *getName();

    static Napi::FunctionReference *constructor;

    VSPluginFunction *vsfunction{nullptr};

  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);
    Napi::Value GetPlugin(const Napi::CallbackInfo &);

    Napi::Value GetName(const Napi::CallbackInfo &);
    Napi::Value GetSignature(const Napi::CallbackInfo &);
    Napi::Value GetReturnType(const Napi::CallbackInfo &);
    Napi::Value Call(const Napi::CallbackInfo &);

    Napi::Value IsVideoInjectable(const Napi::CallbackInfo &);
    Napi::Value IsAudioInjectable(const Napi::CallbackInfo &);

    Core *core{nullptr};
    Plugin *plugin{nullptr};
};
