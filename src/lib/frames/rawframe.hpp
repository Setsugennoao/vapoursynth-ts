#pragma once

#include <sstream>

#include "../core.hpp"

class RawFrame : public Napi::ObjectWrap<RawFrame> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    RawFrame(const Napi::CallbackInfo &);
    ~RawFrame();

    RawFrame *SetInstance(Core *core, VSFrame *vsframe);
    RawFrame *SetInstance(Core *core, const VSFrame *constvsframe);
    static RawFrame *CreateInstance(Core *core, const VSFrame *constvsframe, VSFrame *vsframe);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    Napi::Value GetProps(const Napi::CallbackInfo &);
    Napi::Value GetFlags(const Napi::CallbackInfo &);

    void CloseFrame(const Napi::CallbackInfo &);
    Napi::Value GetIsClosed(const Napi::CallbackInfo &);
    Napi::Value GetIsReadOnly(const Napi::CallbackInfo &);

    void ensureOpen(const Napi::CallbackInfo &);

    const VSFrame *constvsframe{nullptr};
    VSFrame *vsframe{nullptr};
    Core *core{nullptr};
    int flags{-1};
  private:
    Napi::Value GetCore(const Napi::CallbackInfo &);
};
