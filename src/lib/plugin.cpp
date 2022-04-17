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

Napi::Object Plugin::GetPluginObject() {
    Napi::Function proxy = core->proxyFunctions->Get("Plugin").As<Napi::Function>();
    return proxy.Call({this->Value()}).As<Napi::Object>();
}

void Plugin::SetPlugin(Core *core, VSPlugin *vsplugin, RawNode *injectedNode) {
    this->core = core;
    this->vsplugin = vsplugin;
    this->injectedNode = injectedNode;
}

Napi::Object Plugin::CreatePlugin(Core *core, VSPlugin *vsplugin, RawNode *injectedNode) {
    Napi::Object pluginObject = constructor->New({});
    Plugin *plugin = Plugin::Unwrap(pluginObject);
    plugin->SetPlugin(core, vsplugin, injectedNode);
    Napi::Function proxy = core->proxyFunctions->Get("Plugin").As<Napi::Function>();
    return proxy.Call({pluginObject}).As<Napi::Object>();
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

    Napi::Object functionObject = Function::CreateFunction(core, this, vsfunction);

    return functionObject;
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
    return core->GetCoreObject();
}
