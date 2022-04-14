import * as constants from './constants'
import { CoreProxy, PyScriptProxy } from './core'
import { Core, CoreIP } from './types/core'

export * from './constants'
export * from './types/types.d'

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

export const { getOutput, getOutputs, clearOutput, clearOutputs } = (<any>(<unknown>this)).core.__self as CoreIP

export default (<unknown>this) as typeof import('.')
