import * as constants from './constants'
import { CoreProxy, PyScriptProxy } from './core'
import { Core, CoreIP } from './types/core'

export * from './constants'
export { VideoNode, AudioNode, VideoTuple } from './types/core'

/**
 * Gets the singleton Core object.
 *
 * If it is the first time the function is called, the Core will be instantiated with the default options.
 *
 * This is the preferred way to reference the core.
 */
let core: Core
let coreCreationFlags: constants.CoreCreationFlags = constants.CoreCreationFlags.None

const setCoreCreationFlags = (...flags: constants.CoreCreationFlags[]) =>
    (coreCreationFlags = flags.reduce((prev, curr) => prev | curr, constants.CoreCreationFlags.None))

// @ts-ignore
Object.assign(this, {
    get core() {
        return core ?? (core = CoreProxy(coreCreationFlags))
    },
})

export { core, setCoreCreationFlags, PyScriptProxy as PyScript }

export const { getOutput, getOutputs, clearOutput, clearOutputs } = (<any>(<unknown>this)).core as CoreIP

export default (<unknown>this) as typeof import('.')
