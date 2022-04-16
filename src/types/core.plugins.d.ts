import { PresetFormat } from '../constants'
import { Float, Function, Int, Plugin, VideoNode } from './core.d'
import { VideoFormat } from './interfaces'

export class OnlyPluginsCoreProxyI {
    readonly std: Plugin & {
        BlankClip: Function & {
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, fpsden?: Int | null, color?: (Int | Float) | Array<Int> | Array<Float> | null, keep?: boolean | null): VideoNode
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, fpsden?: Int | null, color?: (Int | Float) | Array<Int> | Array<Float> | null, positionalArgs?: { keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, fpsden?: Int | null, positionalArgs?: { color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, positionalArgs?: {fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, positionalArgs?: { fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, positionalArgs?: { length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, width?: Int | null, height?: Int | null, positionalArgs?: { format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, width?: Int | null, positionalArgs?: { height?: Int | null; format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (ref?: VideoNode | null, positionalArgs?: { width?: Int | null; height?: Int | null; format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (positionalArgs: {ref?: VideoNode | null; width?: Int | null; height?: Int | null; format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
        }
    }
}

export class OnlyPluginsVideoNodeProxyI {
    readonly std: Plugin & {
        BlankClip: Function & {
            (width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, fpsden?: Int | null, color?: (Int | Float) | Array<Int> | Array<Float> | null, keep?: boolean | null): VideoNode
            (width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, fpsden?: Int | null, color?: (Int | Float) | Array<Int> | Array<Float> | null, positionalArgs?: { keep?: boolean | null}): VideoNode
            (width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, fpsden?: Int | null, positionalArgs?: { color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, fpsnum?: Int | null, positionalArgs?: {fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, length?: Int | null, positionalArgs?: { fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (width?: Int | null, height?: Int | null, format?: VideoFormat | PresetFormat | null, positionalArgs?: { length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (width?: Int | null, height?: Int | null, positionalArgs?: { format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (width?: Int | null, positionalArgs?: { height?: Int | null; format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
            (positionalArgs: { width?: Int | null; height?: Int | null; format?: VideoFormat | PresetFormat | null; length?: Int | null; fpsnum?: Int | null; fpsden?: Int | null; color?: (Int | Float) | Array<Int> | Array<Float> | null; keep?: boolean | null}): VideoNode
        }
    }
}

export class OnlyPluginsAudioNodeProxyI {
    readonly std: Plugin
}
