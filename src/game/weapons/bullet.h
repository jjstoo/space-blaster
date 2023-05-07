#ifndef BULLET_H
#define BULLET_H

#include "../../physics/physicsobject.h"
#include "../../rendering/renderobject.h"
#include "../graphics.h"
#include "../entity.h"
#include "SDL2/SDL.h"
#include <memory>

class Bullet : public PhysicsObject, public Entity {
  public:
    typedef std::shared_ptr<Bullet> Ptr;

  private:
    static const int pixels = 5;
    std::vector<SDL_Point> getShape_(int x, int y);
    Polygon body_;

    Polygon *getBody() override;
    void collisionWith(Entity *e) override;

  public:
    Bullet(double direction_initial, double v_initial, double x_initial,
           double y_initial, double vx, double vy, RenderEngine *renderEngine,
           PhysicsEngine *physicsEngine, Entity* owner);
    Bullet(int x_initial, int y_initial, int vx, int vy,
           RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
           Entity* owner);
    ~Bullet();
    void update();
    bool alive;
};

#endif
