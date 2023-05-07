#ifndef MININUKE_H
#define MININUKE_H

#include "weapon.h"

class ParticleHandler;
class MiniNuke : public Weapon {
  public:
    MiniNuke(RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
             Entity *owner)
        : Weapon(renderEngine, physicsEngine, owner) {
        weapon_name_ = "Mini Nuke";
        magazine_size_ = 1;
        shots_in_magazine_ = 1;
        shot_speed_ = 2.0f;
        reload_cooldown_ = TICKS_PER_FRAME * 60 * 10;
        fire_rate_cooldown_ = 0;
    }
    void shoot(BulletHandler *bHandler, ParticleHandler *paHandler,
               double direction_rad, int x, int y) override;
};

#endif // BLASTER_H
