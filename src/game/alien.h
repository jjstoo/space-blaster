#ifndef ALIEN_H
#define ALIEN_H

#include "../physics/physicsobject.h"
#include "../rendering/renderobject.h"
#include "SDL2/SDL.h"
#include "graphics.h"
#include "ship.h"
#include <memory>

const static double ALIEN_MAX_SPEED = 0.5;

class Alien : public PhysicsObject {
  public:
    typedef std::shared_ptr<Alien> Ptr;

  public:
    Alien(PhysicsEngine *physicsEngine,
          RenderEngine *renderEngine,
          Ship *player,
          int x_initial, int y_initial,
          double initial_angle,
          int h, int w);
    ~Alien();

    bool alive;
    Polygon *body = nullptr;

    void update();

  private: // functions
    std::vector<SDL_Point> getChassis_(int x, int y);
    void AIStep_();

  private: // variables
    double direction_rad_;
    Ship *player_ = nullptr;
};

#endif // ALIEN_H
