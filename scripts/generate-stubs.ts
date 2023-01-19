import * as dom from 'dts-dom'
import { rename, writeFile } from 'fs/promises'

import { core } from '../src/index'
import { Function } from '../src/types/core'

const namespace = dom.create.namespace('stubs')

const corePlugins = dom.create.class('OnlyPluginsCoreProxyI')
const nodePlugins = dom.create.class('OnlyPluginsNodeProxyI')

const ignoreCase = (a: string, b: string) => +(a.toLocaleLowerCase() > b.toLocaleLowerCase())

const types = {
    Any: dom.type.any,
    Boolean: dom.type.boolean,
    String: dom.type.string,
    Int: dom.create.namedTypeReference('Int'),
    Float: dom.create.namedTypeReference('Float'),
    VideoNode: dom.create.namedTypeReference('VideoNode'),
    AudioNode: dom.create.namedTypeReference('AudioNode'),
    VideoFrame: dom.create.namedTypeReference('VideoFrame'),
    AudioFrame: dom.create.namedTypeReference('AudioFrame'),
    Data: dom.create.union([dom.type.string, dom.create.namedTypeReference('ArrayBufferView')]),
    VideoFormat: dom.create.union([
        dom.create.namedTypeReference('PresetFormat'),
        dom.create.namedTypeReference('VideoFormat'),
    ]),
    GenericFunction: dom.create.union([
        dom.create.namedTypeReference('Function'),
        dom.create.namedTypeReference('JSFunction'),
    ]),
}

const forceIntNames = `
    width;height;nsize;nns;etype;type;threads;cache_size;top;bottom;left;right;fpsnum;fpsden;mthresh;vthresh;hthresh;
    lthresh;rthresh;thresh;threshold;winx;winy;wtop;wbottom;wleft;wright;dxmax;dxmin;dymax;dymin;dw;dy;qual;device;
    ftype;sbsize;smode;sosize;tbsize;tmode;tosize;swin;twin;zmean;resolution;
    matrix;transfer;primaries;range;chromaloc;matrix_in;transfer_in;primaries_in;range_in;chromaloc_in

`
    .split(';')
    .map((x) => x.trim())

const forceStringNames = `
    matrix_s;transfer_s;primaries_s;range_s;chromaloc_s;matrix_in_s;transfer_in_s;primaries_in_s;range_in_s;
    chromaloc_in_s;resample_filter_uv;dither_type;cpu_type
`
    .split(';')
    .map((x) => x.trim())

const forceBooleanNames = ['field', 'dh', 'pscrn', 'list_device', 'info', 'ssystem', 'opt']
const forceVideoFormat = {
    std: ['format'],
    resize: ['format'],
    colorbars: ['format'],
}

function _get_type_from_string(pluginName: string, name: string, type: string, array: boolean) {
    switch (type) {
        case 'vnode':
            return types.VideoNode
        case 'anode':
            return types.AudioNode
        case 'vframe':
            return types.VideoFrame
        case 'aframe':
            return types.AudioFrame
        case 'func':
            return types.GenericFunction
        case 'int':
            // @ts-ignore
            if (pluginName in forceVideoFormat && forceVideoFormat[pluginName].includes(name)) return types.VideoFormat
            if (array || forceIntNames.includes(name)) return types.Int
            else if (forceBooleanNames.includes(name)) return types.Boolean
            return dom.create.union([types.Int, types.Boolean])
        case 'float':
            return types.Float
        case 'data':
            if (forceStringNames.includes(name)) return types.String
            return types.Data
        default:
            return types.Any
    }
}

function _construct_type(pluginName: string, signature: string, positional: boolean) {
    let [name, type, opt] = signature.split(':')

    if (!type) type = name
    if (positional && dom.reservedWords.includes(name)) name += '_'

    const array = type.endsWith('[]')

    if (array) {
        type = type.slice(0, -2)
    }

    let typeT: dom.Type = _get_type_from_string(pluginName, name, type, array)

    if (array) {
        typeT = dom.create.array(typeT)
    }

    if (!!opt) {
        typeT = dom.create.union([typeT, dom.type.null])
    }

    return [name, typeT, !!opt] as [string, dom.Type, boolean]
}

function parseSignature(pluginName: string, signature: string, positional: boolean) {
    return signature
        .split(';')
        .filter((x) => !!x)
        .map((x) => _construct_type(pluginName, x, positional))
}

const toPositional = (parameters: ReturnType<typeof parseSignature>) =>
    parameters.map(([name, type, opt]) => dom.create.parameter(`${name}${opt ? '?' : ''}`, type))

const toNamedArgs = (parameters: ReturnType<typeof parseSignature>) =>
    dom.create.parameter(
        `namedArguments${parameters.every((x) => x[2]) ? '?' : ''}`,
        dom.create.objectType(
            parameters.map(([name, type, opt]) =>
                dom.create.property(name, type, opt ? dom.DeclarationFlags.Optional : dom.DeclarationFlags.None)
            )
        )
    )

function createFunction(pluginName: string, name: string, isNode: boolean, { signature, returnType }: Function) {
    const overloads: dom.ObjectTypeMember[] = []

    const parameters = parseSignature(pluginName, signature, true).slice(+isNode)
    const returntype = parseSignature(
        pluginName,
        returnType == 'any' ? 'clip:vnode' : returnType || 'clip:vnode',
        false
    )[0][1]

    for (const i of Array(parameters.length + 1).keys()) {
        let currparams: dom.Parameter[]

        if (i == 0) {
            currparams = toPositional(parameters)
        } else if (i == parameters.length + 1) {
            if (parameters.length == 1) continue
            currparams = [toNamedArgs(parameters)]
        } else {
            currparams = [...toPositional(parameters.slice(0, -i)), toNamedArgs(parameters.slice(-i))]
        }

        overloads.push(dom.create.callSignature(currparams, returntype))
    }

    const aliasName = `${pluginName}Plugin${name}Function${isNode ? 'Node' : 'Core'}Bound`

    const alias = dom.create.alias(
        aliasName,
        dom.create.intersection([dom.create.namedTypeReference('Function'), dom.create.objectType(overloads)])
    )

    namespace.members.push(alias)

    return dom.create.property(name, dom.create.namedTypeReference(aliasName), dom.DeclarationFlags.ReadOnly)
}

function createPlugin(name: string, isNode: boolean) {
    const functions = dom.create.objectType(
        // @ts-ignore
        core[name].functions.sort().map((func: string) => createFunction(name, func, isNode, core[name][func]))
    )

    const declaration = dom.create.intersection([dom.create.namedTypeReference('Plugin'), functions])

    return Object.assign(dom.create.property(name, declaration, dom.DeclarationFlags.ReadOnly), {
        // @ts-ignore
        jsDocComment: core[name].description,
    })
}

for (const plugin of core.plugins.sort(ignoreCase)) {
    corePlugins.members.push(createPlugin(plugin, false))
    nodePlugins.members.push(createPlugin(plugin, true))
}

namespace.members.push(corePlugins, nodePlugins)

const stubs = `
import { VideoFormat } from './interfaces'
import { PresetFormat } from '../constants'
import { Float, Int, Plugin, VideoNode, AudioNode, Function, JSFunction } from './core.d'

export ${dom.emit(namespace)}
`
    .replace(/\r\n            (?!readonly)/gm, ' ')
    .replace(/\r\n        \}\): (?!readonly)/gm, ' }): ')
    // .replace(/\r\n        \(/gm, ' (')
    .replace(/        \/\*\*\r\n         \*\s(.*)\r\n         \*\//gm, '        /** $1 */')

;(async () => await writeFile('./src/types/core.plugins.temp.d.ts', stubs, { encoding: 'utf-8' }))()
;(async () => await rename('./src/types/core.plugins.temp.d.ts', './src/types/core.plugins.d.ts'))()
