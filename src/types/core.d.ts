import { ColorFamily, PresetFormat, SampleType } from '../constants'
import { Fraction } from '../fractions'
import { AudioNodePP, CorePP, FunctionPP, PluginPP, VideoNodePP } from '../prettyClasses'
import { OnlyPluginsAudioNodeProxyI, OnlyPluginsCoreProxyI, OnlyPluginsVideoNodeProxyI } from './core.plugins'
import { BaseVSProxyI } from './core.proxy'
import { VideoFormat } from './interfaces'
import { Int, MediaOutputT } from './types'

export * from './core.plugins.d'

/**********************************************************************************************************************/
export interface PyScript extends Core {
    new (scriptFilename: string): Core
    new (scriptFilename: string, core: Core): Core
    new (contents: Buffer): Core
    new (contents: Buffer, core: Core): Core
    new (contents: Buffer, scriptFilename: string): Core
    new (contents: Buffer, scriptFilename: string, core: Core): Core
}
/**********************************************************************************************************************/

export type VideoNode = number & VideoNodeI
export type AudioNode = number & AudioNodeI

export interface Function extends BaseVSProxyI {
    readonly signature: string
    readonly returnType: string
}

export interface Plugin extends BaseVSProxyI {
    readonly namespace: string
    readonly description: string
    readonly functions: string[]
}

export type Core = BaseVSProxyI & CoreCppAttributes & CoreStaticAttributes & OnlyPluginsCoreProxyI

/**********************************************************************************************************************/

export interface PluginIP extends Plugin {
    core: CoreIP
    getFunction(name: string): FunctionIP
    isVideoInjectable(): boolean
    isAudioInjectable(): boolean
    prettyPrint: typeof PluginPP
    staticAttributes: PluginStaticAttributes
}

export interface FunctionIP extends Function {
    core: CoreIP
    plugin: PluginIP
    readonly name: string
    Call(...args: any[]): any
    isVideoInjectable(): boolean
    isAudioInjectable(): boolean
    prettyPrint: typeof FunctionPP
    staticAttributes: FunctionStaticAttributes
}

interface VideoNodeIPP extends Omit<VideoNodeI, 'fps'> {
    core: CoreIP
    fps: {
        numerator: Int
        denominator: Int
    }
    prettyPrint: typeof VideoNodePP
    staticAttributes: VideoNodeStaticAttributes
}
export type VideoNodeIP = VideoNodeIPP & VideoNodeCppAttributes
interface AudioNodeIPP extends AudioNodeI {
    core: CoreIP
    prettyPrint: typeof AudioNodePP
    staticAttributes: AudioNodeStaticAttributes
}

export type AudioNodeIP = AudioNodeIPP & AudioNodeStaticAttributes

export interface CoreIP extends Core {
    new (coreCreationFlags: Int, proxies: { [k: string]: (...args: any[]) => any }): CoreIP
    versionNumber: Number
    versionString: String
    getPlugin(name: keyof OnlyPluginsCoreProxyI): PluginIP

    prettyPrint: typeof CorePP
    staticAttributes: CoreStaticAttributes
}

export type PrivateIP = CoreIP | PluginIP | FunctionIP | VideoNodeIP

/**********************************************************************************************************************/

export class CoreCppAttributes {
    numThreads: Int
    maxCacheSize: Int
    readonly plugins: (keyof OnlyPluginsCoreProxyI)[]
    getOutput<T extends MediaOutputT>(index: Int): T
    getOutputs(): MediaOutputT[]
    clearOutput(index: Int): void
    clearOutputs(): void
    queryVideoFormat(
        colorFamily: ColorFamily,
        sample_type: SampleType,
        bitsPerSample: Int,
        subsampling_w: Int,
        subsampling_h: Int
    ): VideoFormat
    getVideoFormat(id: VideoFormat | PresetFormat | Int): VideoFormat

    destroy(): boolean
}

export class PluginCppAttributes {
    readonly functions: string[]
}
export class FunctionCppAttributes {}
export class VideoNodeCppAttributes {
    frameSize: Int
    getFrame(frame: number): Promise<Buffer>
    getFrame(frame: number, buffer: Buffer): Promise<Buffer>
    // getFrame(frame: number, callback: (err: Error, frame: number, buffer: Buffer) => void): null
    getFrame(frame: number, buffer: Buffer, callback: (err: Error, frame: number, buffer: Buffer) => void): null
}

export class AudioNodeCppAttributes {}

/**********************************************************************************************************************/

export class CoreStaticAttributes {
    readonly version: string & { toNumber(): number }
    readonly core: Core
    readonly flags: Int
}

export class PluginStaticAttributes {}

export class FunctionStaticAttributes {}

export class VideoNodeStaticAttributes {
    readonly width: Int
    readonly height: Int
    readonly numFrames: Int
    readonly fps: Fraction
}

export class AudioNodeStaticAttributes {}

/**********************************************************************************************************************/

type IndexableVideoNode = {
    [key: `${Int}`]: VideoNode
    [key: `${Int}:${Int}`]: VideoNode
    [key: `${Int}:`]: VideoNode
    [key: `:${Int}`]: VideoNode
    [key: `${Int}:${Int}:${Int}`]: VideoNode
    [key: `${Int}::${Int}`]: VideoNode
    [key: `:${Int}:${Int}`]: VideoNode
    [key: `::${Int}`]: VideoNode
}

type IndexableAudioNode = {
    [key: `${Int}`]: VideoNode
    [key: `${Int}:${Int}`]: VideoNode
    [key: `${Int}:`]: VideoNode
    [key: `:${Int}`]: VideoNode
    [key: `${Int}:${Int}:${Int}`]: VideoNode
    [key: `${Int}::${Int}`]: VideoNode
    [key: `:${Int}:${Int}`]: VideoNode
    [key: `::${Int}`]: VideoNode
}

declare const nodeOpaqueSymbol: unique symbol
type OpaqueVideoNode = {
    [nodeOpaqueSymbol]: never
    toFixed(): never
    toPrecision(): never
} & IndexableVideoNode

type OpaqueAudioNode = {
    [nodeOpaqueSymbol]: never
    toFixed(): never
    toPrecision(): never
} & IndexableAudioNode

type VideoNodeI = BaseVSProxyI &
    VideoNodeCppAttributes &
    VideoNodeStaticAttributes &
    OnlyPluginsVideoNodeProxyI &
    OpaqueVideoNode
type AudioNodeI = BaseVSProxyI &
    AudioNodeCppAttributes &
    AudioNodeStaticAttributes &
    OnlyPluginsAudioNodeProxyI &
    OpaqueAudioNode

/**********************************************************************************************************************/
