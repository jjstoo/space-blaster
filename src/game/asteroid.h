#ifndef ASTEROID_H
#define ASTEROID_H

#include "../physics/physicsobject.h"
#include "../rendering/renderobject.h"
#include "graphics.h"
#include "entity.h"
#include "SDL2/SDL.h"
#include <memory>
#include <vector>

const static double RADIAL_VARIATION = 0.2;
const static double BREAK_TRESHOLD = 1.3;
const static double CRUMBLE_TRESHOLD = 0.5;

class Asteroid : public PhysicsObject, public Entity {

  public:
    typedef std::shared_ptr<Asteroid> Ptr;

    Asteroid(PhysicsEngine *physicsEngine,
             RenderEngine *renderEngine,
             int x_initial,
             int y_initial,
             int initial_angle,
             double initial_v,
             unsigned int initial_corners,
             unsigned int initial_size);

    ~Asteroid();

    ///
    /// \brief Updates position of the asteroid and checks if it is
    /// out-of-bounds.
    ///
    void update();

    void markDead();
    void markSplit();
    bool isAlive();
    bool isDueSplit();

    void collisionWith(Entity *e) override;
    Polygon *getBody() override;

    int angle;
    int n_collisions;
    unsigned int size;
    Polygon body;
    ScreenPosition screenPos;

  private:
    RenderEngine* renderEngine_;
    bool alive_;
    bool split_ = false;
    int max_r_;
    int min_r_;
    double direction_rad_;

    ///
    /// \brief Returns randomized asteroid shape
    /// \param corners number of corners
    /// \param x center x coordinate
    /// \param y center y coordinate
    /// \return asteroid outline
    ///
    std::vector<SDL_Point> createShape_(int corners, int x, int y) const;

    void handleAsteroidCollision_(Asteroid* a2);

};

#endif
