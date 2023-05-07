#ifndef CANNON_H
#define CANNON_H

#include "weapon.h"

class Cannon : public Weapon {
  public:
    Cannon(RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
           Entity *owner)
        : Weapon(renderEngine, physicsEngine, owner) {
        weapon_name_ = "Cannon";
        magazine_size_ = 15;
        shots_in_magazine_ = 15;
        shot_speed_ = 1.0f;
        reload_cooldown_ = TICKS_PER_FRAME * 60;
        fire_rate_cooldown_ = TICKS_PER_FRAME * 10;
    }
};

#endif // CANNON_H
