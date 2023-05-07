#ifndef DOUBLESHOTGUN_H
#define DOUBLESHOTGUN_H

#include <vector>

#include "../particleHandler.h"
#include "weapon.h"

class DoubleShotgun : public Weapon {
  public:
    DoubleShotgun(RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
                  Entity *owner)
        : Weapon(renderEngine, physicsEngine, owner) {
        weapon_name_ = "Cpruitsis";
        magazine_size_ = 6;
        shots_in_magazine_ = 6;
        shot_speed_ = 1.0f;
        reload_cooldown_ = TICKS_PER_FRAME * 60 * 2;
        fire_rate_cooldown_ = TICKS_PER_FRAME * 8;
    }
    void shoot(BulletHandler *bHandler, ParticleHandler *paHandler,
               double direction_rad, int x, int y) override;

  private:
    std::vector<double> spread_angles;
    std::vector<double> calc_spread_angles(double angle_rad, bool altering);
    // alternating shooting side
    bool has_shot = false;
};

#endif // DOUBLESHOTGUN_H
