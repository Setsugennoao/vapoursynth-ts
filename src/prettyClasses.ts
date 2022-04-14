import { getAttributes } from './utils'

class Core {
    constructor(obj: any) {
        Object.assign(
            this,
            getAttributes(obj, [
                // 'version',
                // 'numThreads',
                // 'maxCacheSize',
                // 'queryVideoFormat',
                // 'getVideoFormat',
                ...obj.plugins,
            ])
        )
    }
}

class Plugin {
    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['namespace', 'description', ...obj.functions]))
    }
}

class Function {
    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['namespace', 'description', ...obj.functions]))
    }
}

class VideoNode {
    constructor(obj: any) {
        Object.assign(this, getAttributes(obj, ['width', 'height', 'numFrames', 'fps']))
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
