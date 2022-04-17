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

    VSMap *inmap = core->vsapi->createMap();

    VSMap *in = core->vsapi->invoke(plugin->vsplugin, getName(), inmap);

    VSNode *node = core->vsapi->mapGetNode(in, "clip", 0, 0);

    Napi::Object videoNodeObject = VideoNode::CreateInstance(core, node);

    return videoNodeObject;
}

Napi::Value Function::GetCore(const Napi::CallbackInfo &info) {
    return core->GetProxyObject();
}

Napi::Value Function::GetPlugin(const Napi::CallbackInfo &info) {
    return plugin->GetProxyObject();
}
