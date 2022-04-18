#include "function.hpp"

#include <string>

Napi::Object Function::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Function", {
        InstanceAccessor<&Function::GetCore>("core"),
        InstanceAccessor<&Function::GetPlugin>("plugin"),
        InstanceMethod<&Function::Call>("Call"),
        InstanceMethod<&Function::IsVideoInjectable>("isVideoInjectable"),
        InstanceMethod<&Function::IsAudioInjectable>("isAudioInjectable"),
        InstanceAccessor<&Function::GetName>("name"),
        InstanceAccessor<&Function::GetSignature>("signature"),
        InstanceAccessor<&Function::GetReturnType>("returnType"),
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("Function", func);

    return exports;
}

Function::Function(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Function>(info) {}

Function *Function::SetInstance(Core *core, Plugin *plugin, VSPluginFunction *vsfunction) {
    this->core = core;
    this->plugin = plugin;
    this->vsfunction = vsfunction;

    return this;
}

Napi::Object Function::CreateInstance(Core *core, Plugin *plugin, VSPluginFunction *vsfunction) {
    return Function::Unwrap(constructor->New({}))->SetInstance(core, plugin, vsfunction)->GetProxyObject();
}

Napi::Object Function::GetProxyObject() {
    return core->proxyFunctions->Get("Function").As<Napi::Function>().Call({this->Value()}).As<Napi::Object>();
}

Napi::FunctionReference *Function::constructor;

Function::~Function() {}

const char *Function::getName() {
    return core->vsapi->getPluginFunctionName(vsfunction);
}

Napi::Value Function::GetName(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), getName());
}

Napi::Value Function::GetSignature(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), core->vsapi->getPluginFunctionArguments(vsfunction));
}

Napi::Value Function::GetReturnType(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), core->vsapi->getPluginFunctionReturnType(vsfunction));
}

Napi::Value Function::IsVideoInjectable(const Napi::CallbackInfo &info) {
    return Napi::Boolean::From(
        info.Env(), std::string{core->vsapi->getPluginFunctionArguments(vsfunction)}.find(":vnode") != std::string::npos
    );
}

Napi::Value Function::IsAudioInjectable(const Napi::CallbackInfo &info) {
    return Napi::Boolean::From(
        info.Env(), std::string{core->vsapi->getPluginFunctionArguments(vsfunction)}.find(":anode") != std::string::npos
    );
}

Napi::Value Function::Call(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    try {
        VSMap *inmap = core->vsapi->createMap();

        if (info.Length() > 0) {
            core->AnyObjectToVSMap(&info[0].As<Napi::Object>(), inmap);
        }

        VSMap *outmap = core->vsapi->invoke(plugin->vsplugin, getName(), inmap);
        core->vsapi->freeMap(inmap);

        const char *error{core->vsapi->mapGetError(outmap)};
        if (error) {
            std::string errorString{error};
            core->vsapi->freeMap(outmap);
            throw Napi::Error::New(Env(), errorString);
        }

        Napi::Value returnValue = core->VSMapToObject(outmap, true);
        core->vsapi->freeMap(outmap);

        return returnValue;
    } catch(Napi::Error err) {
        err.ThrowAsJavaScriptException();
    }

    return env.Null();
}

Napi::Value Function::GetCore(const Napi::CallbackInfo &info) {
    return core->GetProxyObject();
}

Napi::Value Function::GetPlugin(const Napi::CallbackInfo &info) {
    return plugin->GetProxyObject();
}
