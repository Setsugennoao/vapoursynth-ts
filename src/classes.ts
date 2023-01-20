import { getAttributes } from './utils'

export class BasePP {
    constructor(obj: any) {
        this.update(obj)
    }

    update(obj: any) {
        //Object.assign(this, getAttributes(obj, []))
        return this
    }
}

class Core extends BasePP {
    [k: string]: any

    update(obj: any) {
        return Object.assign(this, getAttributes(obj, ['version', 'numThreads', 'maxCacheSize']))
    }
}

class Plugin extends BasePP {
    [k: string]: any

    update(obj: any) {
        return Object.assign(this, getAttributes(obj, ['namespace', 'description', ...obj.functions]))
    }
}

class Function extends BasePP {
    [k: string]: any

    update(obj: any) {
        return Object.assign(this, getAttributes(obj, ['namespace', 'description', ...obj.functions]))
    }
}

class VideoNode extends BasePP {
    [k: string]: any

    update(obj: any) {
        Object.assign(this, getAttributes(obj, ['numFrames']))
        for (const key of ['format', 'width', 'height', 'fps']) {
            this[key] = obj[key] || 'dynamic'
        }
        return this
    }
}

class VideoFrame extends BasePP {
    [k: string]: any

    update(obj: any) {
        Object.assign(this, getAttributes(obj, ['props', 'flags', 'readonly', 'closed']))
        for (const key of ['width', 'height', 'format']) {
            this[key] = obj[key] || 'dynamic'
        }
        return this
    }
}

class AudioNode extends BasePP {}

export {
    Core as CorePP,
    Plugin as PluginPP,
    Function as FunctionPP,
    VideoNode as VideoNodePP,
    AudioNode as AudioNodePP,
    VideoFrame as VideoFramePP,
}
