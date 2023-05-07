#ifndef MACHINEGUN_H
#define MACHINEGUN_H

#include "weapon.h"

class MachineGun : public Weapon {
  public:
    MachineGun(RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
               Entity *owner)
        : Weapon(renderEngine, physicsEngine, owner) {
        weapon_name_ = "Machine gun";
        magazine_size_ = 30;
        shots_in_magazine_ = 30;
        shot_speed_ = 1.2f;
        reload_cooldown_ = TICKS_PER_FRAME * 60 * 5;
        fire_rate_cooldown_ = TICKS_PER_FRAME * 3;
    }
};

#endif // MACHINEGUN_H
