#include "plugin.hpp"

#include "function.hpp"

Napi::Object Plugin::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Plugin", {
        InstanceAccessor<&Plugin::GetCore>("core"),
        InstanceMethod<&Plugin::GetFunction>("getFunction"),
        InstanceAccessor<&Plugin::GetNamespace>("namespace"),
        InstanceAccessor<&Plugin::GetAllFunctions>("functions"),
        InstanceAccessor<&Plugin::GetDescription>("description")
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("Plugin", func);

    return exports;
}

Plugin::Plugin(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Plugin>(info) {}

Napi::Object Plugin::GetProxyObject() {
    return core->proxyFunctions->Get("Plugin").As<Napi::Function>().Call({this->Value()}).As<Napi::Object>();
}

Plugin *Plugin::SetInstance(Core *core, VSPlugin *vsplugin, RawNode *injectedNode) {
    this->core = core;
    this->vsplugin = vsplugin;
    this->injectedNode = injectedNode;

    return this;
}

Napi::Object Plugin::CreateInstance(Core *core, VSPlugin *vsplugin, RawNode *injectedNode) {
    return Plugin::Unwrap(constructor->New({}))->SetInstance(core, vsplugin, injectedNode)->GetProxyObject();
}

Napi::FunctionReference *Plugin::constructor;

Plugin::~Plugin() {}

Napi::Value Plugin::GetNamespace(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), core->vsapi->getPluginNamespace(vsplugin));
}

Napi::Value Plugin::GetDescription(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), core->vsapi->getPluginName(vsplugin));
}

Napi::Value Plugin::GetFunction(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    std::string name = info[0].As<Napi::String>().Utf8Value();

    VSPluginFunction *vsfunction = core->vsapi->getPluginFunctionByName(name.c_str(), vsplugin);

    return Function::CreateInstance(core, this, vsfunction);
}

Napi::Value Plugin::GetAllFunctions(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    Napi::Array functions = Napi::Array::New(env);

    VSPluginFunction *function = NULL;

    while (true) {
        function = core->vsapi->getNextPluginFunction(function, vsplugin);

        if (!function) break;

        functions.Set(functions.Length(), core->vsapi->getPluginFunctionName(function));
    }

    return functions;
}

Napi::Value Plugin::GetCore(const Napi::CallbackInfo &info) {
    return core->GetProxyObject();
}
