import { Console } from 'console'

import { core } from '../index'
import { devNull } from '../utils'

describe(`VapourSynth Core`, () => {
    it(`should verify that recursive core access is available`, () => {
        expect(core.core.core).toBe(core)
    })

    it(`should verify that you can access the number of threds of the core`, () => {
        expect(core.numThreads).toBeTruthy()
    })

    it(`should verify that you can set the number of threds of the core`, () => {
        const expectedNumThreads = 3
        const currentNumThreads = core.numThreads
        core.numThreads = expectedNumThreads
        expect(core.numThreads).toEqual(expectedNumThreads)
        core.numThreads = currentNumThreads
    })

    it(`should verify that you can access the max cache size of the core`, () => {
        expect(core.maxCacheSize).toBeTruthy()
    })

    it(`should verify that you can set the max cache size of the core`, () => {
        const expectedMaxCacheSize = 1024
        const currentMaxCacheSize = core.maxCacheSize
        core.maxCacheSize = expectedMaxCacheSize
        expect(core.maxCacheSize).toEqual(expectedMaxCacheSize)
        core.maxCacheSize = currentMaxCacheSize
    })

    it(`should verify that you can't set bad values to the core max cache size`, () => {
        expect(() => (core.maxCacheSize = 0)).toThrow()
    })

    it(`should verify that you can print the core`, () => {
        expect(() => new Console(devNull()).log(core)).not.toThrow()
    })

    it(`should verify that you can print the core version`, () => {
        expect(() => new Console(devNull()).log(core.version)).not.toThrow()
    })
})
