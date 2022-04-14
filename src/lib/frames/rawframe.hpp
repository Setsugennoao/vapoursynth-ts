#pragma once

#include <sstream>

#include "../core.hpp"

class RawFrame : public Napi::ObjectWrap<RawFrame> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    RawFrame(const Napi::CallbackInfo &);
    ~RawFrame();

    void SetRawNode(Core *core, VSFrame *vsframe, bool writable);
    void ensureOpen(const Napi::CallbackInfo &);

    Napi::Value GetCore(const Napi::CallbackInfo &);
    Napi::Value GetProps(const Napi::CallbackInfo &);
    Napi::Value GetFlags(const Napi::CallbackInfo &);

    Napi::Value GetIsClosed(const Napi::CallbackInfo &);
    Napi::Value GetIsReadOnly(const Napi::CallbackInfo &);

    void CloseFrame(const Napi::CallbackInfo &);

    static Napi::FunctionReference *constructor;

    Core *core{nullptr};
    VSFrame *vsframe{nullptr};
    const VSFrame *constvsframe{nullptr};

  private:
    int flags{-1};
};
