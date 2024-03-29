#pragma once

#include <vapoursynth/VSHelper4.h>
#include <vapoursynth/VSScript4.h>
#include <vapoursynth/VapourSynth4.h>
#include <napi.h>

#include <sstream>

#include "pyscript.hpp"

class Core : public Napi::ObjectWrap<Core> {
  public:
    static Napi::Object Init(Napi::Env, Napi::Object);
    Core(const Napi::CallbackInfo &);
    ~Core();

    Core *SetInstance(const VSAPI *newvsapi, VSCore *newvscore, int creationFlags, PyScript *newpyscript);
    Napi::Object GetProxyObject();

    static Napi::FunctionReference *constructor;
    static bool IsParentOf(Napi::Value &value) { return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value()); }

    Napi::ObjectReference *proxyFunctions;
    Napi::Reference<Napi::Array> *outputs;

    void AnyObjectToVSMap(Napi::Object *object, VSMap *inmap);
    void TypedObjectToVSMap(Napi::Object *object, std::pair<char *, char *> *objectKeyTypes, VSMap *inmap);
    Napi::Value VSMapToObject(VSMap *vsmap, bool shouldFlatten);

    VSCoreInfo GetCoreInfo();

    void setOutput(int index, const Napi::Object value);

    PyScript *pyscript{nullptr};
    const VSAPI *vsapi{nullptr};
    VSCore *vscore{nullptr};
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
    void ClearOutput(const Napi::CallbackInfo &);
    void ClearOutputs(const Napi::CallbackInfo &);

    VSCoreInfo vscoreinfo{nullptr};
    int coreCreationFlags{0};
};

bool NapiIsInteger(Napi::Env &env, Napi::Value &value);
