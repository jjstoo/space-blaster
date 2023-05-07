#include "renderengine.h"

RenderEngine::RenderEngine(Viewport *vp) { vp_ = vp; }

void RenderEngine::addObject(RenderObject *obj) {
    if (obj->getRenderLayer() > TOP_RENDER_LAYER_IDX ||
        obj->getRenderLayer() < BOTTOM_RENDER_LAYER_IDX)
        LOG("RenderObject added with render layer %d out of bounds",
            obj->getRenderLayer());
    else
        objects_[obj->getRenderLayer()].insert(obj);
#if DEBUG_RENDERING
    LOG("%lu objects being rendered", objects_.size());
#endif
}

void RenderEngine::removeObject(RenderObject *obj) {
    objects_[obj->getRenderLayer()].erase(obj);
#if DEBUG_RENDERING
    LOG("%lu objects being rendered", objects_.size());
#endif
}

void RenderEngine::render() {
    for (int i = 0; i < N_RENDER_LAYERS; i++) {
        for (auto obj : objects_[i]) {
            if (obj->isRendering()) {
                obj->render(vp_->getOffsetX(),
                            vp_->getOffsetY());
            }
        }
    }
}
