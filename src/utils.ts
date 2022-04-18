import { createWriteStream } from 'fs'
import os from 'os'

import { Int } from './types/core'
import { DefineProperty } from './types/utils'

export const parseNumber = (n: any) => (isNaN(parseInt(n)) ? NaN : Number(n))

export function parseSliceString(stringSlice: string, length: Int): [number, number, number] {
    let [start, end, step] = stringSlice.split(':').map((s) => parseNumber(s))

    start = isNaN(start) ? 0 : start < 0 ? length + start : start
    end = isNaN(end) ? length : end < 0 ? length + end : end > length ? length : end
    step = isNaN(step) ? 1 : step

    if (step === 0) {
        throw RangeError('Step can not be zero!')
    }

    return [start, end, step]
}

export function* range(start: Int, stop?: Int, step: Int = 1) {
    if (stop == null) {
        stop = start
        start = 0
    }

    for (let i = start; step > 0 ? i < stop : i > stop; i += step) {
        yield i
    }
}

export function devNull() {
    return createWriteStream(os.devNull)
}

export function getAttributes(object: any, attributes: string[]) {
    return Object.fromEntries(attributes.map((attr) => [attr, object[attr]]))
}

export function isFloat(n: number) {
    return typeof n === 'number' && ((n > 0 && n % 1 > 0 && n % 1 < 1) || (n < 0 && n % -1 < 0 && n % -1 > -1))
}

export function roundToPlaces(n: number, places: number) {
    if (!places) return Math.round(n)

    const scalar = Math.pow(10, places)

    return Math.round(n * scalar) / scalar
}

export function defineProperty<Obj extends object, Key extends PropertyKey, PDesc extends PropertyDescriptor>(
    obj: Obj,
    prop: Key,
    val: PDesc
): asserts obj is Obj & DefineProperty<Key, PDesc> {
    Object.defineProperty(obj, prop, val)
}

export const VS_MAKE_VIDEO_ID = (
    colorFamily: number,
    sampleType: number,
    bitsPerSample: number,
    subSamplingW: number,
    subSamplingH: number
) => (colorFamily << 28) | (sampleType << 24) | (bitsPerSample << 16) | (subSamplingW << 8) | (subSamplingH << 0)
