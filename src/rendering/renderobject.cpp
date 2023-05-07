#include "renderobject.h"
#include "renderengine.h"

RenderObject::RenderObject() = default;

RenderObject::RenderObject(RenderEngine *renderEngine) {
    init(renderEngine);
}

RenderObject::RenderObject(RenderEngine *renderEngine, int layer) {
    init(renderEngine, layer);
}

RenderObject::~RenderObject() { renderEngine_->removeObject(this); }

void RenderObject::init(RenderEngine *renderEngine, int layer) {
    renderEngine_ = renderEngine;
    layer_ = layer;
    renderEngine_->addObject(this);
    initialized_ = true;
}

void RenderObject::setRenderLayer(int layer) {
    layer_ = layer;

    if (initialized_) {
        renderEngine_->removeObject(this);
        renderEngine_->addObject(this);
    }
}

void RenderObject::setRendering(bool enable) { rendering_ = enable; }
int RenderObject::getRenderLayer() { return layer_; }
void RenderObject::setInView(bool in_view) { in_view_ = in_view; }
bool RenderObject::isInView() { return in_view_; }

