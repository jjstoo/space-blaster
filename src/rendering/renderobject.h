#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "../blaster.h"

class ViewPort;
class RenderEngine;
class RenderObject {

  public:

    RenderObject();

    ///
    /// \brief RenderObject is an abstraction for any rendereable object
    /// \param renderEngine RenderEngine instance
    ///
    RenderObject(RenderEngine *renderEngine);

    ///
    /// \brief RenderObject is an abstraction for any rendereable object
    /// \param renderEngine RenderEngine instance
    /// \param layer initial render layer
    ///
    RenderObject(RenderEngine *renderEngine, int layer);

    void init(RenderEngine* renderEngine, int layer=TOP_RENDER_LAYER_IDX - 1);

    virtual ~RenderObject();

    ///
    /// \brief render function to be called on every frame by
    /// RenderEngine if rendering_ is set to true
    ///
    virtual void render(int offset_x, int offset_y) = 0;

    ///
    /// \brief enableRendering enables or disables rendering
    /// \param enable render status to set
    ///
    void setRendering(bool enable);

    ///
    /// \brief isRendering returns current rendering status
    /// true if the object is rendered
    /// false if rendering is disabled
    /// \return render status
    ///
    bool isRendering() { return rendering_; }


    /// 
    /// \brief sets the object in-view status
    ///
    void setInView(bool in_view);

    /// 
    /// \brief checks if the object is in view
    ///
    bool isInView();

    ///
    /// \brief isStatic checks if the object is rendered statically
    /// \return true if the object is static
    ///
    bool isStatic() { return static_; }

    ///
    /// \brief getRenderLayer checks the objects render layer
    /// \return render layer
    ///
    int getRenderLayer();

    void setRenderLayer(int layer);

  protected:
    int layer_ = TOP_RENDER_LAYER_IDX - 1;
    bool rendering_ = true;
    bool in_view_ = true;
    bool static_ = false;
    bool initialized_ = false;
    RenderEngine *renderEngine_;
};

#endif // RENDEROBJECT_H
