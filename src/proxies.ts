import util from 'util'

import { BasePP, CorePP, PluginPP, VideoFramePP, VideoNodePP } from './classes'
import { Fraction } from './fractions'
import {
    AudioNodeIP,
    Core,
    CoreIP,
    FunctionIP,
    Int,
    PluginIP,
    PyScript,
    stubs,
    VideoFrameIP,
    VideoNode,
    VideoNodeIP,
} from './types/core'
import { VideoFrame } from './types/core.d'
import { getAttributes, parseSliceString, range } from './utils'

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
                            return (wrapper[key] = nativeFunctionProxy(cppobj[key], cppobj, key))
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

const VideoFrameProxy = (frame: VideoFrameIP) =>
    createProxy(frame, frame, {
        __PP: VideoFramePP,
        __getter: (name: keyof stubs.OnlyPluginsNodeProxyI) => {
            console.log(name)
        },
        // get fps() {
        //     return new Fraction(frame.fps.numerator, frame.fps.denominator)
        // },
    })

const VideoNodeProxy = (node: VideoNodeIP) =>
    createProxy(node, node, {
        __PP: VideoNodePP,
        __getter: (name: keyof stubs.OnlyPluginsNodeProxyI) => {
            if (node.core.plugins.includes(name)) {
                return node.core.getPlugin(name, node)
            }

            const asnumber = Number(name)

            if (!isNaN(asnumber) || name.includes(':')) {
                if (name.includes(':')) {
                    let [start, stop, step] = parseSliceString(name, node.numFrames)

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
                } else {
                    if (Number.isInteger(asnumber)) {
                        if (!Number.isSafeInteger(asnumber)) throw Error('Index overflows!')

                        const n = asnumber < 0 ? node.numFrames + asnumber : asnumber

                        if (n < 0 || (node.numFrames > 0 && n >= node.numFrames)) {
                            throw RangeError('List index out of bounds')
                        }

                        return node.core.std.Trim({ clip: <VideoNode>(<unknown>node), first: n, length: 1 })
                    }

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

        async *frames(_prefetch?: Int, _backlog?: Int, close: boolean = true) {
            const prefetch = _prefetch ?? node.core.numThreads
            const backlog = Math.max(_backlog ?? prefetch * 3, prefetch)

            function* _enumPromises(): Generator<[Int, Promise<VideoFrame>], null, any> {
                for (const i of range(node.numFrames)) {
                    yield [i, node.getFrameAsync(i)]
                }

                return null
            }

            const enumPromise = _enumPromises()

            let finished = false
            let running = 0

            const reorder: Record<Int, Promise<VideoFrame | Error>> = {}

            function _refill() {
                if (finished) return

                while (running < prefetch && Object.keys(reorder).length < backlog) {
                    _request_next()
                }
            }

            function _request_next() {
                if (finished) return

                const ni = enumPromise.next()

                if (ni.done) {
                    return (finished = true)
                }

                running++

                const [idx, fut] = ni.value

                reorder[idx] = fut.then(_finished).catch(_finishedExc)
            }

            function _finished(f: VideoFrame) {
                setImmediate(() => {
                    if (!finished) _refill()
                })
                return f
            }

            function _finishedExc(err: Error) {
                finished = true

                return err
            }

            _refill()

            const curr = {} as { result?: VideoFrame | Error }

            let sidx = 0
            try {
                while (!finished || Object.keys(reorder).length > 0 || running > 0) {
                    if (reorder[sidx] == null) {
                        continue
                    }

                    curr.result = await reorder[sidx]

                    delete reorder[sidx]

                    _refill()

                    sidx++
                    running--

                    const isError = curr.result instanceof Error

                    try {
                        if (isError) {
                            throw curr.result
                        }

                        // prettier-ignore
                        yield [sidx, curr.result]
                    } finally {
                        if (close && !isError) {
                            // @ts-ignore
                            curr.result.close()
                            delete curr.result
                        }
                    }
                }
            } finally {
                finished = true
                for await (const fut of Object.values(reorder)) {
                    if (fut instanceof Error) continue
                    fut.close()
                }
            }
        },
    })

const FunctionProxy = (func: FunctionIP) =>
    createProxy(
        function (...args: any[]) {
            let funcArguments = {}

            const argNames = func.signature.split(';').map((x) => x.split(':')[0])

            if (args && args.length) {
                if (typeof args[args.length - 1] === 'object') {
                    funcArguments = args[args.length - 1]
                    args = args.slice(0, -1)
                }

                const posKeyVal = args.map((val, i, _arr) => [argNames[i], val])

                const positional = Object.fromEntries(posKeyVal)

                funcArguments = Object.assign(funcArguments, positional)
            }

            funcArguments = Object.fromEntries(
                Object.entries(funcArguments).map(([key, value]) => [
                    key.endsWith('_') ? key.substring(0, key.length - 1) : key,
                    value,
                ])
            )

            try {
                return func.Call(funcArguments)
            } catch (e: any) {
                throw e
            }
        },
        func,
        {
            __PP: null,
            __setPrintInstance: () =>
                Object.assign(
                    new Function(`return function ${func.name}() {}`)(),
                    getAttributes(func, ['signature', 'returnType'])
                ),
        }
    )

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
        VideoFrame: VideoFrameProxy,
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
