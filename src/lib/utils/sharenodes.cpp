#include "../core.hpp"

#include "../nodes/rawnode.hpp"
#include "../nodes/videonode.hpp"
// #include "../nodes/audionode.hpp"

#include <vapoursynth/VSHelper4.h>

static const VSFrame *VS_CC ShareVideoNodeGetFrame(int n, int activationReason, void *instanceData, void **frameData, VSFrameContext *frameCtx, VSCore *core, const VSAPI *vsapi) {
    RawNode *rawnode = reinterpret_cast<RawNode *>(instanceData);

    char errMsg[1024];
    const VSFrame *src = rawnode->core->vsapi->getFrame(n, rawnode->vsnode, errMsg, sizeof(errMsg));

    if (!src) {
        std::ostringstream ss;
        ss << "Encountered error getting frame " << n << ": " << errMsg;
        throw Napi::Error::New(rawnode->Env(), ss.str());
    }

    int width = rawnode->core->vsapi->getFrameWidth(src, 0);
    int height = rawnode->core->vsapi->getFrameHeight(src, 0);
    const VSVideoFormat *fi = rawnode->core->vsapi->getVideoFrameFormat(src);

    VSFrame *dst = vsapi->newVideoFrame(fi, width, height, src, core);

    memcpy(
        vsapi->getWritePtr(dst, 0), rawnode->core->vsapi->getReadPtr(src, 0),
        VideoNode::getFrameSize(width, height, fi) / fi->bitsPerSample
    );

    rawnode->core->vsapi->freeFrame(src);

    return dst;
}

static void VS_CC ShareVideoNodeFree(void *instanceData, VSCore *core, const VSAPI *vsapi) {
    delete instanceData;
}

static VSNode *ShareVideoNode(Core *core, VideoNode *vnode) {
    if (vnode->rawnode->core->vscore == core->vscore) {
        return vnode->rawnode->vsnode;
    }

    return core->vsapi->createVideoFilter2(
        "ShareDifferentCoreNode", vnode->vsvideoinfo,
        ShareVideoNodeGetFrame, ShareVideoNodeFree,
        fmParallel, {}, 0, vnode->rawnode, core->vscore
    );
}
