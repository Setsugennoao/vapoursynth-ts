#include "core.hpp"

Napi::Object PyScript::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "PyScript", { InstanceAccessor<&PyScript::GetCore>("core") });

    coreObject = new Napi::ObjectReference();
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("PyScript", func);

    return exports;
}

PyScript::PyScript(const Napi::CallbackInfo &info) : Napi::ObjectWrap<PyScript>(info) {
    Napi::Env env = info.Env();
    int argLength = info.Length();
    std::string path { "" };

    if (argLength < 1 || argLength > 3) {
        Napi::TypeError::New(env, "Wrong number of arguments!").ThrowAsJavaScriptException();
        return;
    }

    if (info[0].IsString()) {
        path = info[0].As<Napi::String>().Utf8Value();
    } else if (info[0].IsBuffer()) {
        if (argLength > 1) {
            if (info[1].IsString()) {
                path = info[1].As<Napi::String>().Utf8Value();
            }
        }
    } else {
        Napi::TypeError::New(env, "The first argument has to be either a buffer or string path of the script!")
            .ThrowAsJavaScriptException();
        return;
    }

    *coreObject = Napi::Weak(info[1].As<Napi::Object>());

    Core *core = Core::Unwrap(coreObject->Value())->SetInstance(NULL, NULL, NULL, this);

    vssapi = getVSScriptAPI(VSSCRIPT_API_VERSION);
    if (!vssapi) {
        Napi::Error::New(env, "Failed to initialize Vapoursynth VSScript API!").ThrowAsJavaScriptException();
        return;
    }

    vsscript = vssapi->createScript(core->vscore);
    if (!vsscript) {
        Napi::Error::New(env, "Failed to initialize Vapoursynth VSScrip!").ThrowAsJavaScriptException();
        return;
    }

    int error;
    if (info[0].IsString()) {
        vssapi->evalSetWorkingDir(vsscript, 1);
        error = vssapi->evaluateFile(vsscript, path.c_str());
    } else {
        char *buffer = info[0].As<Napi::Buffer<char>>().Data();
        error = vssapi->evaluateBuffer(vsscript, buffer, path.c_str());
    }

    if (error) {
        std::ostringstream ss;
        ss << "Script evaluation failed:\n\tPath: " << path ? path : "<Buffer>";

        const char *errorMsg = vssapi->getError(vsscript);
        vssapi->freeScript(vsscript);
        vsscript = nullptr;

        if (errorMsg)
            ss << "\n\tError: " << errorMsg;

        Napi::Error::New(env, ss.str()).ThrowAsJavaScriptException();

        return;
    }
}

PyScript::~PyScript() {
    if (vsscript) {
        vssapi->freeScript(vsscript);
    }
}

Napi::Value PyScript::GetCore(const Napi::CallbackInfo &info) {
    return coreObject->Value();
}

Napi::ObjectReference *PyScript::coreObject;
Napi::FunctionReference *PyScript::constructor;
