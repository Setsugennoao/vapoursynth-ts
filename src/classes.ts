import { getAttributes } from './utils'

class Core {
    [k: string]: any

    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['version', 'numThreads', 'maxCacheSize']))
    }
}

class Plugin {
    [k: string]: any

    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['namespace', 'description', ...obj.functions]))
    }
}

class Function {
    [k: string]: any

    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['namespace', 'description', ...obj.functions]))
    }
}

class VideoNode {
    [k: string]: any

    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['numFrames']))
        for (const key of ['format', 'width', 'height', 'fps']) {
            this[key] = obj[key] || 'dynamic'
        }
    }
}

class AudioNode {
    constructor(obj: any) {
        //Object.assign(this, getAttributes(obj, []))
    }
}

export {
    Core as CorePP,
    Plugin as PluginPP,
    Function as FunctionPP,
    VideoNode as VideoNodePP,
    AudioNode as AudioNodePP,
}
