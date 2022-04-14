import { ColorFamily, SampleType } from '../constants'

export interface VideoFormat {
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
