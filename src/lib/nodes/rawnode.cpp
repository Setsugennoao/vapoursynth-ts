#include "rawnode.hpp"

Napi::Object RawNode::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RawNode", {
        InstanceAccessor<&RawNode::GetCore>("core"),
        InstanceMethod<&RawNode::GetFrames>("frames")
    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("RawNode", func);

    return exports;
}

RawNode::RawNode(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RawNode>(info) {}

void RawNode::SetRawNode(Core *core, VSNode *vsnode) {
    this->core = core;
    this->vsnode = vsnode;
}

Napi::FunctionReference *RawNode::constructor;

RawNode::~RawNode() {
    if (core && vsnode) {
        core->vsapi->freeNode(vsnode);
    }
}

Napi::Value RawNode::GetFrames(const Napi::CallbackInfo &info) {
    return Napi::Value::From(info.Env(), "frames"); // TODO
}

Napi::Value RawNode::GetCore(const Napi::CallbackInfo &info) {
    return core->GetCoreObject();
}