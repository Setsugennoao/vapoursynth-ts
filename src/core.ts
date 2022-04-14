import util from 'util'

import { Fraction } from './fractions'
import { CorePP, PluginPP, VideoNodePP } from './prettyClasses'
import {
    AudioNodeIP, Core, CoreIP, FunctionIP, OnlyPluginsCoreProxyI, OnlyPluginsVideoNodeProxyI, PluginIP, PrivateIP,
    PyScript, VideoNodeIP
} from './types/core'
import { GetCreateProxyT } from './types/core.proxy'
import { Int } from './types/types'
import { getAttributes } from './utils'

export const { Core: _Core, PyScript: _PyScript } = require('bindings')('vapoursynthts.node') as {
    Core: CoreIP
    PyScript: PyScript
}

const createProxy = <T extends PrivateIP, S extends { __self: any; __PP: T['prettyPrint']; __setPrintInstance?: any }>(
    otherobj: S
): GetCreateProxyT<T> => {
    let instance: any,
        self: { [key: string]: any } = Object.assign(
            {
                __printInstance: undefined,
                __setPrintInstance: () =>
                    self.__printInstance ?? ((<any>self.__printInstance) = new self.__PP(instance)),
            },
            otherobj
        )

    instance = Object.assign(
        new Proxy(self.__self, {
            get: (_, name: string) => {
                switch (true) {
                    case name in self.__self:
                        return self.__self[name]
                    case name in self:
                        return self[name]
                    case '__getter' in self:
                        return self.__getter(self, name)
                    default:
                        throw ReferenceError(`No attribute with the name ${name} exists.`)
                }
            },
            set: (_, name: string, newValue: any) => {
                try {
                    self.__self[name] = newValue
                    return true
                } catch (error) {
                    if (self.hasOwnProperty(name)) {
                        self[name] = newValue

                        return true
                    }
                    throw error
                }
            },
        }),
        {
            [util.inspect.custom]: () => self.__setPrintInstance(),
            toString: () => JSON.stringify(self.__setPrintInstance()),
        }
    )

    // @ts-ignore
    return (otherobj = null), instance
}

const VideoNodeProxy = (node: VideoNodeIP) =>
    createProxy({
        __self: node,
        __PP: VideoNodePP,
        __getter: (_: any, name: keyof OnlyPluginsVideoNodeProxyI) => {
            if (node.core.plugins.includes(name)) {
                return node.core.getPlugin(name)
            }

            throw ReferenceError(
                `No attribute with the name ${name} exists. Did you mistype a plugin namespace or not install it?`
            )
        },
        get fps() {
            return new Fraction(node.fps.numerator, node.fps.denominator)
        },
    })

const FunctionProxy = (func: FunctionIP) =>
    createProxy({
        __PP: Function,
        __self: (...args: any[]) => func.Call(...args), // TODO: returnType check
        __setPrintInstance: () =>
            Object.assign(
                new Function(`return function ${func.name}() {}`)(),
                getAttributes(func, ['signature', 'returnType'])
            ),
    })

const PluginProxy = (plugin: PluginIP, injected_arg: null | VideoNodeIP | AudioNodeIP = null) =>
    createProxy({
        __PP: PluginPP,
        __self: plugin,
        __injected_arg: injected_arg,
        __getter: (_: any, name: string) => {
            if (plugin.functions.includes(name)) {
                return FunctionProxy(plugin.getFunction(name))
            }

            throw ReferenceError(`No attribute with the name ${name} exists. Did you mistype a function name?`)
        },
        isVideoInjectable: () => {
            for (const func in plugin.functions) {
                if (plugin.getFunction(func).isVideoInjectable()) {
                    return true
                }
            }
            return false
        },
        isAudioInjectable: () => {
            for (const func in plugin.functions) {
                if (plugin.getFunction(func).isAudioInjectable()) {
                    return true
                }
            }
            return false
        },
    })

// @ts-ignore
export const CoreProxy = (
    coreCreationFlags: Int = 0,
    core: CoreIP = new _Core(coreCreationFlags, {
        RawNode: (obj) => obj,
        RawFrame: (obj) => obj,
        FrameProps: (obj) => obj,
        VideoFormat: (obj) => obj,
        VideoNode: VideoNodeProxy,
        VideoFrame: (obj) => obj,
        AudioFormat: (obj) => obj,
        AudioNode: (obj) => obj,
        AudioFrame: (obj) => obj,
        Function: FunctionProxy,
        Plugin: PluginProxy,
        Core: CoreProxy,
    })
) => {
    const _coreInstance = (<unknown>createProxy({
        __PP: CorePP,
        __self: core,
        __getter: (_: any, name: keyof OnlyPluginsCoreProxyI) => {
            if (core.plugins.includes(name)) {
                return core.getPlugin(name)
            }

            throw ReferenceError(
                `No attribute with the name ${name} exists. Did you mistype a plugin namespace or not install it?`
            )
        },
    })) as Core

    return Object.assign(_coreInstance, {
        core: _coreInstance,
        version: Object.assign(core.versionString, {
            toString: () => core.versionString,
            toNumber: () => +core.versionNumber,
            valueOf: () => core.versionNumber,
            [util.inspect.custom]: () => core.versionString.toString(),
        }),
    })
}

// @ts-ignore
export const PyScriptProxy: PyScript = (...args: any[]) => {
    if (args.length == 1 || (args.length == 2 && typeof args[0] === 'string' && args[1] instanceof Buffer)) {
        args.push(CoreProxy())
    }

    // @ts-ignore
    return new _PyScript(...args).core
}
