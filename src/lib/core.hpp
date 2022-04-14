#pragma once

#include <VSHelper4.h>
#include <VSScript4.h>
#include <VapourSynth4.h>
#include <napi.h>

#include <sstream>

#include "pyscript.hpp"

class Core : public Napi::ObjectWrap<Core> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    Core(const Napi::CallbackInfo &);
    ~Core();

    void SetCore(const VSAPI *newvsapi, VSCore *newvscore, int creationFlags, PyScript *newpyscript);
    Napi::Object GetCoreObject();
    VSCoreInfo GetCoreInfo();

    static Napi::FunctionReference *constructor;
    Napi::ObjectReference *proxyFunctions;

    const VSAPI *vsapi{nullptr};
    VSCore *vscore{nullptr};
    PyScript *pyscript{nullptr};

  private:
    Napi::Value Destroy(const Napi::CallbackInfo &);
    Napi::Value GetAllPlugins(const Napi::CallbackInfo &);
    Napi::Value GetPlugin(const Napi::CallbackInfo &);

    Napi::Value GetCoreVersion(const Napi::CallbackInfo &);
    Napi::Value GetCoreVersionNumber(const Napi::CallbackInfo &);
    Napi::Value GetCoreCreationFlags(const Napi::CallbackInfo &);

    Napi::Value GetNumThreads(const Napi::CallbackInfo &);
    void SetNumThreads(const Napi::CallbackInfo &, const Napi::Value &);

    Napi::Value GetMaxCacheSize(const Napi::CallbackInfo &);
    void SetMaxCacheSize(const Napi::CallbackInfo &, const Napi::Value &);

    Napi::Object queryVideoFormat(VSColorFamily colorFamily, VSSampleType sampleType, int bitsPerSample, int subsamplingW, int subsamplingH);
    Napi::Value QueryVideoFormat(const Napi::CallbackInfo &);
    Napi::Object getVideoFormat(uint32_t id);
    Napi::Value GetVideoFormat(const Napi::CallbackInfo &);

    Napi::Value GetOutput(const Napi::CallbackInfo &);
    Napi::Value GetOutputs(const Napi::CallbackInfo &);
    Napi::Value ClearOutput(const Napi::CallbackInfo &);
    Napi::Value ClearOutputs(const Napi::CallbackInfo &);

    VSMap *ObjectToVSMap(Napi::Object *object);
    Napi::Object VSMapToObject(Napi::Env env, VSMap *vsmap);

    VSCoreInfo vscoreinfo{nullptr};
    int coreCreationFlags{0};
};
