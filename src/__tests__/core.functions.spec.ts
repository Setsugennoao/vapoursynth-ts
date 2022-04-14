import { Console } from 'console'

import { core } from '../index'
import { devNull } from '../utils'

describe(`VapourSynth Core Functions`, () => {
    it(`should verify that function can be accessed`, () => {
        expect(core.std.BlankClip).toBeTruthy()
    })

    it(`should verify that accessing a non-existant function throws an error`, () => {
        const t = () => {
            return (<any>core.std).thisfunctiondoesntexists
        }
        expect(t).toThrow(ReferenceError)
        expect(t).toThrowError(/No attribute.*thisfunctiondoesntexists.*function.*/g)
    })

    it(`should verify that you can print a plugin function`, () => {
        expect(() => new Console(devNull()).log(core.std.BlankClip)).not.toThrow()
    })
})
