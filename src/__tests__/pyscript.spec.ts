import { appendFile, open } from 'fs/promises'

import { PyScript, VideoTuple } from '../index'
import { range } from '../utils'

describe(`PyScript`, () => {
    const script = 'import vapoursynth as vs;vs.core.std.BlankClip(color=[128,150,255]).set_output(0)#\r\n#'
    const pyscript = new PyScript(Buffer.from(script, 'utf-8'))

    it(`should get the number of frames of the blank clip`, () => {
        const { clip } = pyscript.getOutput<VideoTuple>(0)
        expect(clip.numFrames).toBe(240)
    })

    it(`should get the frame and write to buffer (sync)`, () => {
        const { clip } = pyscript.getOutput<VideoTuple>(0)
        expect(clip.frameSize).toEqual(921600)

        clip.getFrame(0, Buffer.alloc(clip.frameSize), (err, frameNum, buffer) => {
            expect(err).toBe(null)
            expect(frameNum).toBe(0)
            expect(buffer.length).toBe(clip.frameSize)
            expect(buffer.length).toBe(921600)
            expect(buffer[0]).toEqual(128)
        })
    })

    // it(`should get the frame and allocate the correct buffer (sync)`, () => {
    //     const { clip } = pyscript.getOutput<VideoTuple>(0)
    //     expect(clip.frameSize).toEqual(921600)

    //     clip.getFrame(0, (err, frameNum, buffer) => {
    //         expect(err).toBe(null)
    //         expect(frameNum).toBe(0)
    //         expect(buffer.length).toBe(clip.frameSize)
    //         expect(buffer.length).toBe(921600)
    //         expect(buffer[0]).toEqual(128)
    //     })
    // })

    it(`should get the frame and write to buffer (async)`, async () => {
        const { clip } = pyscript.getOutput<VideoTuple>(0)
        expect(clip.frameSize).toEqual(921600)

        const buffer = await clip.getFrame(0, Buffer.alloc(clip.frameSize))

        expect(buffer.length).toBe(clip.frameSize)
        expect(buffer.length).toBe(921600)
        expect(buffer[0]).toEqual(128)
    })

    it(`should get the frame and allocate the correct buffer (async)`, async () => {
        const { clip } = pyscript.getOutput<VideoTuple>(0)
        expect(clip.frameSize).toEqual(921600)

        const buffer = await clip.getFrame(0)

        expect(buffer.length).toBe(clip.frameSize)
        expect(buffer.length).toBe(921600)
        expect(buffer[0]).toEqual(128)
    })

    it(`should get the frame and write it to file`, async () => {
        const { clip } = pyscript.getOutput<VideoTuple>(0)
        expect(clip.frameSize).toEqual(921600)

        const file = await open('./src/__tests__/test.raw', 'w+')

        for (const frame of range(10)) {
            await appendFile(file, await clip.getFrame(frame), 'binary')
        }
    })
})
