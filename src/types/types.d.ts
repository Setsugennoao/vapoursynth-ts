import { AudioNode, VideoNode } from './core'

export type Int = number
export type Float = number

interface RawNodeI {}
interface VideoNodeI extends RawNodeI {}
interface AudioNodeI extends RawNodeI {}

interface VideoTuple {
    clip: VideoNode
    alpha: VideoNode | null
    altOutput: Int
}

type MediaOutputT = VideoTuple | AudioNode
