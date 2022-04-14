import { createWriteStream } from 'fs'
import os from 'os'

import { Int } from './types/types.d'
import { DefineProperty } from './types/utils'

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
