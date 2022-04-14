#include "core.hpp"

#include "plugin.hpp"
#include "function.hpp"

#include "formats.hpp"

#include "./frames/rawframe.hpp"

#include "./nodes/rawnode.hpp"
#include "./nodes/videonode.hpp"

Napi::Object Core::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Core", {
        InstanceMethod<&Core::Destroy>("destroy"),
        InstanceMethod<&Core::GetPlugin>("getPlugin"),
        InstanceMethod<&Core::QueryVideoFormat>("queryVideoFormat"),
        InstanceMethod<&Core::GetVideoFormat>("getVideoFormat"),
        InstanceMethod<&Core::GetOutput>("getOutput"),
        InstanceMethod<&Core::GetOutputs>("getOutputs"),
        InstanceMethod<&Core::ClearOutput>("clearOutput"),
        InstanceMethod<&Core::ClearOutputs>("clearOutputs"),
        InstanceAccessor<&Core::GetAllPlugins>("plugins"),
        InstanceAccessor<&Core::GetCoreCreationFlags>("flags"),
        InstanceAccessor<&Core::GetCoreVersion>("versionString"),
        InstanceAccessor<&Core::GetCoreVersionNumber>("versionNumber"),
        InstanceAccessor<&Core::GetNumThreads, &Core::SetNumThreads>("numThreads"),
        InstanceAccessor<&Core::GetMaxCacheSize, &Core::SetMaxCacheSize>("maxCacheSize")
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("Core", func);

    return exports;
}

Core::Core(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Core>(info) {
    Napi::Env env = info.Env();

    int creationFlags = info[0].As<Napi::Number>().Int32Value();

    proxyFunctions = new Napi::ObjectReference();
    *proxyFunctions = Napi::Persistent(info[1].As<Napi::Object>());

    SetCore(NULL, NULL, creationFlags, NULL);
}

Core::~Core() {}

Napi::Value Core::Destroy(const Napi::CallbackInfo &info) {
    if (vscore && vscore) {
        vsapi->freeCore(vscore);
        return Napi::Boolean::From(info.Env(), true);
    }
    return Napi::Boolean::From(info.Env(), false);
}

void Core::SetCore(const VSAPI *newvsapi, VSCore *newvscore, int creationFlags, PyScript *newpyscript) {
    if (!newvsapi) {
        if (!vsapi) {
            vsapi = getVapourSynthAPI(VAPOURSYNTH_API_VERSION);
        }
    } else {
        vsapi = newvsapi;
    }

    if (!newvscore) {
        if (!vscore) {
            vscore = vsapi->createCore(creationFlags);
            coreCreationFlags = creationFlags;
        }
    } else {
        if (vscore) {
            vsapi->freeCore(vscore);
        }
        vscore = newvscore;
    }

    if (newpyscript) {
        if (!pyscript) {
            pyscript = newpyscript;
        } else {
            pyscript->~PyScript();
            pyscript = newpyscript;
        }
    }
}

VSCoreInfo Core::GetCoreInfo() {
    vsapi->getCoreInfo(vscore, &vscoreinfo);
    return vscoreinfo;
}

Napi::FunctionReference *Core::constructor;

Napi::Object Core::GetCoreObject() {
    Napi::Function proxy = proxyFunctions->Get("Core").As<Napi::Function>();
    return proxy.Call({ this->Env().Null(), this->Value()}).As<Napi::Object>().Get("__self").As<Napi::Object>();
}

Napi::Value Core::GetPlugin(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    std::string name = info[0].As<Napi::String>().Utf8Value();

    VSPlugin *vsplugin = vsapi->getPluginByNamespace(name.c_str(), vscore);

    Napi::Object pluginObject = Plugin::CreatePlugin(this, vsplugin);

    return pluginObject;
}

Napi::Value Core::GetAllPlugins(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    Napi::Array plugins = Napi::Array::New(env);

    VSPlugin *plugin = NULL;

    while (true) {
        plugin = vsapi->getNextPlugin(plugin, vscore);

        if (!plugin) break;

        plugins.Set(plugins.Length(), vsapi->getPluginNamespace(plugin));
    }

    return plugins;
}

Napi::Value Core::GetCoreVersion(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, GetCoreInfo().versionString);
}

Napi::Value Core::GetCoreVersionNumber(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, GetCoreInfo().core);
}

Napi::Value Core::GetCoreCreationFlags(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, coreCreationFlags);
}

Napi::Value Core::GetNumThreads(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, GetCoreInfo().numThreads);
}

void Core::SetNumThreads(const Napi::CallbackInfo &info, const Napi::Value &value) {
    Napi::Env env = info.Env();

    if (!value || !value.IsNumber()) {
        Napi::Error::New(env, "Core number of threads has to be a number!").ThrowAsJavaScriptException();
        return;
    }

    int newvalue = value.As<Napi::Number>().Int32Value();

    if (newvalue <= 0) {
        Napi::Error::New(env, "Core number of threads has to be a positive number bigger than 0!").ThrowAsJavaScriptException();
        return;
    }

    vsapi->setThreadCount(newvalue, vscore);
}

Napi::Value Core::GetMaxCacheSize(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    int64_t size = (GetCoreInfo().maxFramebufferSize + 1024 * 1024 - 1) / (1024 * 1024);

    return Napi::Number::New(env, size);
}

void Core::SetMaxCacheSize(const Napi::CallbackInfo &info, const Napi::Value &value) {
    Napi::Env env = info.Env();

    if (!value || !value.IsNumber()) {
        Napi::Error::New(env, "Core max cache size has to be a number!").ThrowAsJavaScriptException();
        return;
    }

    int newvalue = value.As<Napi::Number>().Int32Value();

    if (newvalue <= 0) {
        Napi::Error::New(env, "Core max cache size has to be a positive number bigger than 0!").ThrowAsJavaScriptException();
        return;
    }

    vsapi->setMaxCacheSize(newvalue * 1024 * 1024, vscore);
}

Napi::Object Core::queryVideoFormat(VSColorFamily colorFamily, VSSampleType sampleType, int bitsPerSample, int subsamplingW, int subsamplingH) {
    VSVideoFormat *vsformat{nullptr};

    if (!vsapi->queryVideoFormat(vsformat, colorFamily, sampleType, bitsPerSample, subsamplingW, subsamplingH, vscore)) {
        Napi::Error::New(Env(), "Invalid format properties specified!").ThrowAsJavaScriptException();
        return Env().Null().As<Napi::Object>();
    }

    return VideoFormat::CreateVideoFormat(this, vsformat);
}

Napi::Object Core::getVideoFormat(uint32_t id) {
    VSVideoFormat *vsformat{nullptr};
    if (!vsapi->getVideoFormatByID(vsformat, id, vscore)) {
        Napi::Error::New(Env(), "Invalid format id specified!").ThrowAsJavaScriptException();
        return Env().Null().As<Napi::Object>();
    }
    return VideoFormat::CreateVideoFormat(this, vsformat);
}

Napi::Value Core::GetVideoFormat(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsNumber()) {
        Napi::Error::New(Env(), "Format id has to be a number!").ThrowAsJavaScriptException();
        return env.Null();
    }

    return getVideoFormat(info[0].As<Napi::Number>().Uint32Value());
}

Napi::Value Core::QueryVideoFormat(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() != 5) {
        Napi::Error::New(env, "Wrong number of arguments!").ThrowAsJavaScriptException();
        return env.Null();
    }

    int args[5]{};

    for (int i = 0; i < 5; i++) {
        if (!info[i].IsNumber()) {
            Napi::Error::New(env, "ColorFamily, sampleType and bitsPerSample have to be numbers!").ThrowAsJavaScriptException();
            return env.Null();
        } else if (i == 3 || i == 4) {
            args[i] = 0;
        } else {
            args[i] = info[i].As<Napi::Number>().Uint32Value();
        }
    }

    return queryVideoFormat((VSColorFamily)args[0], (VSSampleType)args[1], args[2], args[3], args[4]);
}

Napi::Value Core::GetOutput(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    int index = info[0].As<Napi::Number>().Int32Value();

    Napi::Object outputObject = Napi::Object::New(env);

    if (pyscript) {
        VSNode *vsnode = pyscript->vssapi->getOutputNode(pyscript->vsscript, index);
        Napi::Object clipObj = VideoNode::CreateNode(this, vsnode);

        VSNode *vsalphanode = pyscript->vssapi->getOutputAlphaNode(pyscript->vsscript, index);
        Napi::Value alphaObj = vsalphanode ? VideoNode::CreateNode(this, vsalphanode) : env.Null();

        int vsaltoutput = pyscript->vssapi->getAltOutputMode(pyscript->vsscript, index);
        Napi::Number altOutputObj = Napi::Number::New(env, vsaltoutput);

        outputObject.Set("clip", clipObj);
        outputObject.Set("alpha", alphaObj);
        outputObject.Set("altOutput", altOutputObj);
    } else {
        outputObject.Set("clip", env.Null());
        outputObject.Set("alpha", env.Null());
        outputObject.Set("altOutput", env.Null());
    }

    return outputObject;
}

Napi::Value Core::GetOutputs(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Object::New(env);
}

Napi::Value Core::ClearOutput(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Object::New(env);
}

Napi::Value Core::ClearOutputs(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Object::New(env);
}

VSMap *Core::ObjectToVSMap(Napi::Object *object) {
    return vsapi->createMap();
}

Napi::Object Core::VSMapToObject(Napi::Env env, VSMap *vsmap) {
    return Napi::Object::New(env);
}

/**********************************************************************************************************************/

Napi::Object Register(Napi::Env env, Napi::Object exports) {
    RawNode::Init(env, exports);
    RawFrame::Init(env, exports);
    // FrameProps::Init(env, exports);

    VideoFormat::Init(env, exports);
    VideoNode::Init(env, exports);
    // VideoFrame::Init(env, exports);

    // AudioFormat::Init(env, exports);
    // AudioNode::Init(env, exports);
    // AudioFrame::Init(env, exports);

    Function::Init(env, exports);
    Plugin::Init(env, exports);
    Core::Init(env, exports);

    PyScript::Init(env, exports);

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Register);
