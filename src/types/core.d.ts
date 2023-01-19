import { WriteStream } from 'fs'
import util from 'util'

import { AudioNodePP, CorePP, FunctionPP, PluginPP, VideoNodePP } from '../classes'
import { ColorFamily, PresetFormat, SampleType } from '../constants'
import { Fraction } from '../fractions'
import { stubs } from './core.plugins'
import { VideoFormat } from './interfaces'

export * from './core.plugins.d'

/**********************************************************************************************************************/

export type Int = number
export type Float = number

/**
 * This class is returned by getOutput if the output is video.
 */
export interface VideoTuple {
    /** A VideoNode-instance containing the video information. */
    clip: VideoNode
    /** A VideoNode-instance containing the alpha plane. */
    alpha: VideoNode | null
    /**
     * An integer with the alternate output mode to be used.
     *
     * May be ignored if no meaningful mapping exists.
     */
    altOutput: Int
}

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

/**
 * Represents a video clip. The class itself supports indexing and slicing to perform trim, Reverse and SelectEvery operations.

 * Several operators are also defined for the VideoNode class: addition appends clips and multiplication repeats them.

 * Note that slicing and indexing always return a new VideoNode object and not a VideoFrame.

 * For TypeScript users: You will get a number from normal operators and will have to do `as VideoNode` to get typehinting back.

 * This is a limitation of the language as it doesn't support operator overloading and they can only be done on primitives.
 */

export interface VideoNode extends VideoNodeI {}
export interface VideoFrame extends VideoFrameI {}

export interface AudioNode extends AudioNodeI {}

export interface Function extends BaseVSProxyI {
    readonly signature: string
    readonly returnType: string
}

export interface JSFunction extends BaseVSProxyI {
    (...args: any[]): any
}

export interface Plugin extends BaseVSProxyI {
    readonly namespace: string
    readonly description: string
    readonly functions: string[]
}

/**
 * This is the VapourSynth main core, you can access plugins and modify environment variables.

 * The Core class uses a singleton pattern for this environment. Use the core attribute to obtain an instance.

 * All loaded plugins are exposed as attributes of the core object.

 * These attributes in turn hold the functions contained in the plugin.

 * Use plugins() to obtain a full list of all currently loaded plugins you may call this way.

 * Most operations in the VapourSynth library are performed through this singleton core object.

 * This core may load plugins, which all end up in their own unit, or namespace, so to say, to avoid naming conflicts in the contained functions.

 * For this reason you call a plugin function with core.unit.Function().

 * All arguments to functions have names that are lowercase and all function names are CamelCase.

 * Unit names are also lowercase and usually short. This is good to remember as a general rule.
 */
type CoreBase = BaseVSProxyI & CoreCppAttributes & CoreStaticAttributes & stubs.OnlyPluginsCoreProxyI
export interface Core extends CoreBase {}

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
    getPlugin(name: keyof stubs.OnlyPluginsCoreProxyI, injectedNode?: VideoNode | AudioNode): PluginIP

    prettyPrint: typeof CorePP
    staticAttributes: CoreStaticAttributes
}

export type PrivateIP = CoreIP | PluginIP | FunctionIP | VideoNodeIP

/**********************************************************************************************************************/

export class CoreCppAttributes {
    /**
     * The number of concurrent threads used by the core. Can be set to change the number.
     * @throws {@link RangeError | RangeError} when setting to a value less than one.
     */
    numThreads: Int
    /**
     * Set the upper framebuffer cache size after which memory is aggressively freed. The value is in megabytes.
     * @throws {@link RangeError | RangeError} when setting to a value less than one.
     */
    maxCacheSize: Int
    /**
     * Readonly property containing all names of loaded plugins.
     */
    readonly plugins: (keyof stubs.OnlyPluginsCoreProxyI)[]
    /**
     * Get a previously set output node. Throws an error if the index hasn’t been set.

     * Will return either a {@link VideoTuple | VideoTuple} containing the clip, alpha and the altOutput setting for video output or an {@link AudioNode | AudioNode} for audio output.
     * @throws {@link RangeError | RangeError} when there is not a node at the specified index.
     */
    getOutput<T extends VideoTuple | AudioNode>(index: Int): T
    /**
     * Return a read-only mapping of all outputs registered on the current core.

     * The mapping will automatically update when a new output is registered on the core.
     */
    getOutputs(): (VideoTuple | AudioNode)[]
    /**
     * Clears a clip previously set for output.
     * @throws {@link RangeError | RangeError} when there is not a node at the specified index.
     */
    clearOutput(index: Int): void
    /**
     * Clears all clips set for output in the current core.
     */
    clearOutputs(): void
    /**
     * Retrieve a Format object corresponding to the format information, Invalid formats throw an exception.
     */
    queryVideoFormat(
        colorFamily: ColorFamily,
        sample_type: SampleType,
        bitsPerSample: Int,
        subsampling_w: Int,
        subsampling_h: Int
    ): VideoFormat
    /**
     * Retrieve a Format object corresponding to the specified id.
     * @throws {@link Error | Error} if the id is invalid.
     */
    getVideoFormat(id: VideoFormat | PresetFormat | Int): VideoFormat
    /**
     * Calling this will destroy the Core instance and the environment along all obects related to it.
     * @throws {@link Error | Error} if the core was already destroyed.
     */
    destroy(): boolean
}

export class PluginCppAttributes {
    readonly functions: string[]
}
export class FunctionCppAttributes {}
export class VideoNodeCppAttributes {
    frameSize: Int
    /**
     * Set the clip to be accessible for output. This is the standard way to specify which clip(s) to output.

     * All VapourSynth tools (vsvfw, vsfs, vspipe) use the clip in index 0.

     * It’s possible to specify an additional containing the alpha to output at the same time.

     * Currently only vspipe takes alpha into consideration when outputting.

     * The altOutput argument is for optional alternate output modes. Currently it controls the FOURCCs used for VFW-style output with certain formats.
     */
    setOutput(index: Int, alpha?: VideoNode, altOutput?: Int): void
    /**
     * Returns a VideoFrame from position n.
     * @throws {@link Error | Error} if there is an error during the frame request.
     */
    getFrame(frameNumber: number): VideoFrame
    /**
     * Returns a promise of which the result will be a VideoFrame.
     * @throws {@link Error | Error} if there is an error during the frame request.
     */
    getFrameAsync(frameNumber: number): Promise<VideoFrame>
    /**
     * Write the whole clip to the specified file handle. It is possible to pipe to stdout by specifying process.stdout as the file.

     * @param y4m YUV4MPEG2 headers will be added when true
     * @param progressUpdateCallback will be reported the current progress to this callback function.
     * @param prefetch is only for debugging purposes and should never need to be changed.
     * @param backlog is only for debugging purposes and should never need to be changed.
     */
    output(
        fileStream: WriteStream,
        y4m?: boolean,
        progressUpdateCallback?: (current: Int, total: Int) => void,
        prefetch?: Int,
        backlog?: Int
    ): void
}

export class VideoFrameCppAttributes {
    close: () => void
}

export class AudioNodeCppAttributes {}

/**********************************************************************************************************************/

export class CoreStaticAttributes {
    /**
     * Returns version information as a string.

     * Type coercion to number and calling `toNumber` will give the core version as a number.
     */
    readonly version: string & { toNumber(): number }
    /**
     * Recursive access to the singleton core instance for this environment.
     */
    readonly core: Core
    /**
     * Core creation flags, see the VapourSynth {@link https://github.com/vapoursynth/vapoursynth/blob/R58/src/core/vscore.cpp#L1757 | code} {@link https://github.com/vapoursynth/vapoursynth/blob/R58/src/core/vscore.cpp#L1763-L1764 | usages}
     */
    readonly flags: Int
}

export class PluginStaticAttributes {}

export class FunctionStaticAttributes {}

export class VideoNodeStaticAttributes {
    /**
     * The width of the video. This value will be 0 if the width and height can change between frames.
     */
    readonly width: Int
    /**
     * The height of the video. This value will be 0 if the width and height can change between frames.
     */
    readonly height: Int
    /**
     * The number of frames in the clip.
     */
    readonly numFrames: Int
    /**
     * The framerate represented as a Fraction. It is 0/1 when the clip has a variable framerate.

     * `numerator`: The numerator of the framerate. If the clip has variable framerate, the value will be 0.

     * `denominator`: The denominator of the framerate. If the clip has variable framerate, the value will be 1.
     */
    readonly fps: Fraction
    /**
     * A VideoFormat object describing the frame data. If the format can change between frames (variable clip), this value is null.
     */
    readonly format: VideoFormat
    /**
     * Special flags set for this clip. This attribute should normally be ignored.
     */
    readonly flags: Int
    frames(
        prefetch?: Int | null,
        backlog?: Int | null,
        close?: boolean
    ): AsyncGenerator<[number, VideoFrame], void, undefined>
}

export class VideoFrameStaticAttributes {}

export class AudioNodeStaticAttributes {}

/**********************************************************************************************************************/

type IndexableVideoNode = {
    [key: Int]: VideoNode
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
    [key: Int]: AudioNode
    [key: `${Int}`]: AudioNode
    [key: `${Int}:${Int}`]: AudioNode
    [key: `${Int}:`]: AudioNode
    [key: `:${Int}`]: AudioNode
    [key: `${Int}:${Int}:${Int}`]: AudioNode
    [key: `${Int}::${Int}`]: AudioNode
    [key: `:${Int}:${Int}`]: AudioNode
    [key: `::${Int}`]: AudioNode
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
    stubs.OnlyPluginsNodeProxyI &
    OpaqueVideoNode

type VideoFrameI = BaseVSProxyI & VideoFrameCppAttributes & VideoFrameStaticAttributes

type AudioNodeI = BaseVSProxyI &
    AudioNodeCppAttributes &
    AudioNodeStaticAttributes &
    stubs.OnlyPluginsNodeProxyI &
    OpaqueAudioNode

/**********************************************************************************************************************/

export type GetCreateProxyT<T extends PrivateIP> = StaticInstance<T, T['prettyPrint']> & T['staticAttributes']

export interface StaticInstance<T, S> extends BaseVSProxyI {
    __self: T
    __innerInstance: T
    __printInstance: S | undefined
    __setPrintInstance(): S

    [util.inspect.custom]: () => S
}

export interface BaseVSProxyI {
    core: Core
    toString(): string
}
