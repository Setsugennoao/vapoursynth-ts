import { Console } from 'console'

import { core } from '../index'
import { devNull } from '../utils'

describe(`VapourSynth Core plugins`, () => {
    it(`should verify that plugins get loaded`, () => {
        expect(core.plugins.length).toBeGreaterThan(0)
    })

    it(`should verify that the standard plugin got loaded`, () => {
        expect(core.std).toBeTruthy()
    })

    it(`should verify that accessing non-existant plugins throws an error`, () => {
        const t = () => {
            return (<any>core).thisplugindoesntexists
        }
        expect(t).toThrow(ReferenceError)
        expect(t).toThrowError(/No attribute.*thisplugindoesntexists.*plugin.*/g)
    })

    it(`should verify that you can print a plugin`, () => {
        expect(() => new Console(devNull()).log(core.std)).not.toThrow()
    })
})
