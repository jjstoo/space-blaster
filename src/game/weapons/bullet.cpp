#include "bullet.h"
#include "../../game.h"
#include "../graphics.h"

#include <vector>

#ifdef _WIN32
#include <math.h>
#endif

/// Destructor
Bullet::~Bullet() {}

/// Initialization
Bullet::Bullet(double direction_initial, double v_initial, double x_initial,
               double y_initial, double vx, double vy,
               RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
               Entity* owner) :
   PhysicsObject(physicsEngine, 2.0, x_initial, y_initial, true, -1.0f,
                    false),
    Entity(owner,false, BULLET) {

    // Set collision properties
    setCollidable(true);
    ENABLE_COLLISION(ASTEROID);
    ENABLE_COLLISION(SHIP);

    double vx_total = v_initial * cos(direction_initial) + vx;
    double vy_total = v_initial * sin(direction_initial) + vy;
    setSpeedXY(vx_total, vy_total);
    alive = true;

    body_.init(renderEngine, getShape_(x_initial, y_initial), x_initial, y_initial);
    body_.setColor(SDL_Color{255, 51, 51, 255});
    body_.setRenderType(POINT);
}

Bullet::Bullet(int x_initial, int y_initial, int vx, int vy,
               RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
               Entity* owner)
    : PhysicsObject(physicsEngine, 2.0, x_initial, y_initial, true, -1.0f,
                    false),
    Entity(owner, false, BULLET) {

    // Set collision properties
    setCollidable(true);
    ENABLE_COLLISION(ASTEROID);
    ENABLE_COLLISION(SHIP);

    setSpeedXY(vy, vx);
    alive = true;

    body_.init(renderEngine, getShape_(x_initial, y_initial), x_initial, y_initial);
    body_.setColor(SDL_Color{255, 51, 51, 255});
    body_.setRenderType(POINT);
}

std::vector<SDL_Point> Bullet::getShape_(int x, int y) {
    std::vector<SDL_Point> points = {SDL_Point{x, y - 1}, SDL_Point{x - 1, y},
                                     SDL_Point{x + 1, y}, SDL_Point{x, y + 1},
                                     SDL_Point{x, y}};

    return points;
}

void Bullet::update() {
    body_.moveAbsolute(getPosX(), getPosY());
    if (CoordinateUtils::check_out_of_bounds(body_.x, body_.y)) {
        alive = false;
    }
}
void Bullet::collisionWith(Entity *e) {
    if (e->getType() != BULLET) {
        if (e->getType() == SHIP) {
            if (getOwnerId() != e->getId())
                alive = false;
        } else {
            alive = false ;
            if (owner_->getType() == SHIP) {
                if (e->getType() == ASTEROID) {
                    owner_->addToScore(((Asteroid *) e)->size);
                }
            }
        }
    }
}
Polygon *Bullet::getBody() { return &body_; }
