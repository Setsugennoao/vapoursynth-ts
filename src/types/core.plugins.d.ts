import { PresetFormat } from '../constants'
import { Function, Plugin, VideoNode } from './core.d'
import { VideoFormat } from './interfaces'
import { Float, Int } from './types'

type _getOverride<args0 extends any[], args1 extends {}> = Function & {
    (...positionalArgs: args0): VideoNode
    (namedArgs: args1): VideoNode
}

export class OnlyPluginsCoreProxyI {
    readonly std: Plugin & {
        BlankClip: _getOverride<
            [
                ref?: VideoNode | null,
                width?: Int | null,
                height?: Int | null,
                format?: VideoFormat | PresetFormat | null,
                length?: Int | null,
                fpsnum?: Int | null,
                fpsden?: Int | null,
                color?: (Int | Float) | Array<Int> | Array<Float> | null,
                keep?: boolean | null
            ],
            {
                ref?: VideoNode | null
                width?: Int | null
                height?: Int | null
                format?: VideoFormat | PresetFormat | null
                length?: Int | null
                fpsnum?: Int | null
                fpsden?: Int | null
                color?: (Int | Float) | Array<Int> | Array<Float> | null
                keep?: boolean | null
            }
        >
    }
}

export class OnlyPluginsVideoNodeProxyI {
    readonly std: Plugin & {
        BlankClip: _getOverride<
            [
                width?: Int | null,
                height?: Int | null,
                format?: VideoFormat | PresetFormat | null,
                length?: Int | null,
                fpsnum?: Int | null,
                fpsden?: Int | null,
                color?: (Int | Float) | Array<Int> | Array<Float> | null,
                keep?: boolean | null
            ],
            {
                width?: Int | null
                height?: Int | null
                format?: VideoFormat | PresetFormat | null
                length?: Int | null
                fpsnum?: Int | null
                fpsden?: Int | null
                color?: (Int | Float) | Array<Int> | Array<Float> | null
                keep?: boolean | null
            }
        >
    }
}

export class OnlyPluginsAudioNodeProxyI {
    readonly std: Plugin
}
