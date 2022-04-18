import { ColorFamily, SampleType } from '../constants'
import { Int } from './core'

export interface VideoFormat {
    id: Int
    colorFamily: ColorFamily
    sampleType: SampleType
    bytesPerSample: number
    bitsPerSample: number
    subSamplingW: number
    subSamplingH: number
    numPlanes: number
}

export interface AudioFormat {
    sampleType: SampleType
    bitsPerSample: number
    bytesPerSample: number
    numChannels: number
    channelLayout: number
}
