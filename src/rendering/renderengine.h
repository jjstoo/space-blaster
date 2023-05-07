#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include "../game/viewport.h"
#include "renderobject.h"
#include <set>

class RenderEngine {
  public:
    ///
    /// \brief RenderEngine handler class for calling render functions of
    /// RenderObject instances on each frame
    ///
    RenderEngine(Viewport *vp);

    ///
    /// \brief addObject adds an object to the render queue
    /// \param obj RenderObject instance
    ///
    void addObject(RenderObject *obj);

    ///
    /// \brief removeObject remove object from render queue
    /// \param obj RenderObject instance
    ///
    void removeObject(RenderObject *obj);

    ///
    /// \brief render runs render tasks on each render queue object
    ///
    void render();

  private:
    std::set<RenderObject *> objects_[N_RENDER_LAYERS];
    Viewport *vp_;
};

#endif // RENDERENGINE_H
