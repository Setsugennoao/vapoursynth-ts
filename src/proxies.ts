import util from 'util'

import { CorePP, PluginPP, VideoNodePP } from './classes'
import { Fraction } from './fractions'
import {
    AudioNodeIP, Core, CoreIP, FunctionIP, Int, PluginIP, PyScript, stubs, VideoNode, VideoNodeIP
} from './types/core'
import { getAttributes, parseSliceString } from './utils'

export const { Core: CoreBinding, PyScript: PyScriptBinding } = require('bindings')('vapoursynthts.node') as {
    Core: CoreIP
    PyScript: PyScript
}

const nativeFunctionProxy = (func: any, target: any, key: any) =>
    new Proxy(func, { apply: (_, __, args) => target[key].apply(target, args) })

const createProxy = (objtowrap: any, cppobj: any, values: any): any => {
    let instance: any

    const wrapper: any = Object.assign(
        {
            __self: cppobj,
            __printInstance: undefined,
            __setPrintInstance: () => wrapper.__printInstance ?? (wrapper.__printInstance = new wrapper.__PP(instance)),
        },
        values
    )

    instance = new Proxy(objtowrap, {
        get: (target, key, receiver) => {
            switch (true) {
                case key in wrapper:
                    return wrapper[key]
                case key in cppobj: {
                    const property = cppobj[key]
                    switch (typeof property) {
                        case 'function':
                            return nativeFunctionProxy(cppobj[key], cppobj, key)
                        default:
                            return property
                    }
                }
                case key in target:
                    return target[key]
                case '__getter' in wrapper:
                    return wrapper.__getter(key)
                default:
                    throw ReferenceError(`No attribute with the name ${key.toString()} exists.`)
            }
        },
        set: (target, key, newValue) => {
            switch (typeof key) {
                case 'symbol':
                    return (target[key] = newValue)
                default:
                    return (wrapper[key] = newValue)
            }
        },
    })

    Object.assign(instance, {
        [util.inspect.custom]: () => wrapper.__setPrintInstance(),
        toString: () => JSON.stringify(wrapper.__setPrintInstance()),
    })

    return instance
}

const VideoNodeProxy = (node: VideoNodeIP) =>
    createProxy(node, node, {
        __PP: VideoNodePP,
        __getter: (name: keyof stubs.OnlyPluginsNodeProxyI) => {
            if (node.core.plugins.includes(name)) {
                return node.core.getPlugin(name, node)
            }

            if (!Number.isNaN(parseInt(name)) || name.includes(':')) {
                if (name.includes(':')) {
                    let [start, stop, step] = parseSliceString(name, node.numFrames)

                    console.log({ start, stop, step })

                    if (step < 0) [start, stop] = [stop, start]

                    let ret = <VideoNode>(<unknown>node)

                    if (step > 0 && stop !== null) stop -= 1
                    if (step < 0 && start !== null) start += 1

                    if (start !== null && stop !== null) {
                        ret = node.core.std.Trim({ clip: ret, first: start, last: stop })
                    } else if (start !== null) {
                        ret = node.core.std.Trim({ clip: ret, first: start })
                    } else if (stop !== null) {
                        ret = node.core.std.Trim({ clip: ret, last: stop })
                    }

                    if (step < 0) {
                        ret = node.core.std.Reverse({ clip: ret })
                    }

                    if (Math.abs(step) != 1) {
                        ret = node.core.std.SelectEvery({ clip: ret, cycle: Math.abs(step), offsets: [0] })
                    }

                    return ret
                } else if (!Number.isNaN(parseInt(name))) {
                    if (!Number.isSafeInteger(parseInt(name))) throw Error('Index overflows!')
                    const n = parseInt(name) < 0 ? node.numFrames + parseInt(name) : parseInt(name)

                    if (n < 0 || (node.numFrames > 0 && n >= node.numFrames)) {
                        throw RangeError('List index out of bounds')
                    }

                    return node.core.std.Trim({ clip: <VideoNode>(<unknown>node), first: n, length: 1 })
                } else {
                    throw TypeError('Index must be int or string slice!')
                }
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
    createProxy((...args: any[]) => func.Call(...args), func, {
        __PP: null,
        __setPrintInstance: () =>
            Object.assign(
                new Function(`return function ${func.name}() {}`)(),
                getAttributes(func, ['signature', 'returnType'])
            ),
    })

const PluginProxy = (plugin: PluginIP, injected_arg: null | VideoNodeIP | AudioNodeIP = null) =>
    createProxy(plugin, plugin, {
        __PP: PluginPP,
        __injected_arg: injected_arg,
        __getter: (name: string) => {
            if (plugin.functions.includes(name)) {
                return plugin.getFunction(name)
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
export const CoreProxy: (flags?: Int, core?: CoreIP) => Core = (
    coreCreationFlags: Int = 0,
    core: CoreIP = new CoreBinding(coreCreationFlags, {
        RawNode: (obj) => obj,
        RawFrame: (obj) => obj,
        RawFrameEditable: (obj) => obj,
        FrameProps: (obj) => obj,
        FramePropsEditable: (obj) => obj,
        VideoFormat: (obj) => obj.toObject(),
        VideoNode: VideoNodeProxy,
        VideoFrame: (obj) => obj,
        VideoFrameEditable: (obj) => obj,
        AudioFormat: (obj) => obj,
        AudioNode: (obj) => obj,
        AudioFrame: (obj) => obj,
        AudioFrameEditable: (obj) => obj,
        Function: FunctionProxy,
        Plugin: PluginProxy,
        Core: CoreProxy,
    })
) => {
    const _coreInstance = createProxy(core, core, {
        __PP: CorePP,
        __getter: (name: keyof stubs.OnlyPluginsCoreProxyI) => {
            if (core.plugins.includes(name)) {
                return core.getPlugin(name)
            }

            throw ReferenceError(
                `No attribute with the name ${name} exists. Did you mistype a plugin namespace or not install it?`
            )
        },
    }) as Core

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
        // @ts-ignore
        args.push(CoreProxy().__self)
    }

    // @ts-ignore
    return new PyScriptBinding(...args).core
}
