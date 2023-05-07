#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <memory>

#include "../physics/physicsobject.h"
#include "SDL2/SDL.h"

class Viewport {
  public:
    typedef std::shared_ptr<Viewport> Ptr;

    ///
    /// \brief Viewport handles viewport translations and movement
    /// \param buffer buffer zone from the viewport edges that causes camera
    /// movement
    ///
    Viewport(int buffer);
    ~Viewport();

    ///
    /// \brief track_object sets object to track with the viewport
    /// \param obj PhysicsObject instance
    ///
    void track_object(PhysicsObject *obj);

    ///
    /// \brief Updates viewport position
    ///
    void update();

    int getOffsetX() { return -offset_x_; }
    int getOffsetY() { return -offset_y_; }

    ///
    /// \brief Returns viewport for rendering
    /// \return viewport
    ///
    SDL_Rect *get();

    bool isPointInView(int x, int y);

  private:
    int offset_x_ = 0;
    int offset_y_ = 0;
    int offset_x_max_;
    int offset_y_max_;
    int buffer_;

    SDL_Rect *vp_;
    PhysicsObject *target_;
};

#endif // VIEWPORT_H
