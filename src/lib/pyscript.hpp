#pragma once
#include "core.hpp"

class PyScript : public Napi::ObjectWrap<PyScript> {
  public:
    PyScript(const Napi::CallbackInfo &);
    ~PyScript();

    Napi::Value GetCore(const Napi::CallbackInfo &);

    static Napi::Object Init(Napi::Env, Napi::Object);

    const VSSCRIPTAPI *vssapi{nullptr};
    VSScript *vsscript{nullptr};

  private:
    static Napi::ObjectReference *coreObject;
    static Napi::FunctionReference *constructor;
};
