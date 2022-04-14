import { Console } from 'console'

import { core } from '../index'
import { devNull } from '../utils'

describe(`VapourSynth Nodes`, () => {
    it(`should verify that nodes get created`, () => {
        const clip = core.std.BlankClip()
        expect(clip).toBeTruthy()
    })

    it(`should verify that you can print a VideoNode`, () => {
        expect(() => new Console(devNull()).log(core.std.BlankClip())).not.toThrow()
    })

    // const clip = core.std.BlankClip({ format: vs.YUV444P8 })
    // const clip1 = core.std.BlankClip({ format: vs.YUV444P8 })

    // const clip2 = clip + clip1 // add two clips together
    // const clip3 = clip2 * 5 + 100 // loop 5 and then adding 100 blank frames (with clip.std.BlankClip)
    // const clip4 = clip3 << 8 // from 8 bits to 16 (with core.resize.Point)
    // const clip5 = -clip4 as VideoNode // reverses the clip
    // const clip6 = clip5['::-1'] // same as above
    // const clip7 = (clip6[':1500'] + clip6['60:']) as VideoNode

    // console.log(clip7)
})
