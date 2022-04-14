import util from 'util'

import { Core, PrivateIP } from './core.d'

export type GetCreateProxyT<T extends PrivateIP> = StaticInstance<T, T['prettyPrint']> & T['staticAttributes']

export interface StaticInstance<T, S> extends BaseVSProxyI {
    __self: T
    __innerInstance: T
    __printInstance: S | undefined
    __setPrintInstance(): S

    [util.inspect.custom]: () => S
}

export interface BaseVSProxyI {
    core: Core
    toString(): string
}
