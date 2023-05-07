#include "doubleshotgun.h"

#include "SDL2/SDL.h"
#include "weapon.h"

// Edited shoot function for spread-shot
void DoubleShotgun::shoot(BulletHandler *bHandler, ParticleHandler *paHandler,
                          double direction_rad, int x, int y) {
    if ((!isReloading()) && (getShotsLeft() > 0)) {

        if ((SDL_GetTicks() - last_shot_start_ticks_) > fire_rate_cooldown_) {

            double vx = physicsOwner_->getVelX();
            double vy = physicsOwner_->getVelY();

            // Gun offset
            int px_outward_front = -6;
            int px_outward_sides = 5;

            // Alternating shot
            has_shot = !has_shot;
            if (has_shot) {
                px_outward_sides = -px_outward_sides;
            }
            double dx = px_outward_front * cos(direction_rad) -
                        px_outward_sides * sin(direction_rad);
            double dy = px_outward_front * sin(direction_rad) +
                        px_outward_sides * cos(direction_rad);
            x = static_cast<int>(x + dx);
            y = static_cast<int>(y + dy);
            // Alternating speed
            double speed_d = (rand() % 2 - 1) * 0.1;
            // Spreading shot
            spread_angles = calc_spread_angles(direction_rad, has_shot);
            bHandler->addBullet(Bullet::Ptr(
                new Bullet(spread_angles[0], shot_speed_ - speed_d, x, y, vx,
                           vy, renderEngine_, physicsEngine_, (Entity*)physicsOwner_)));
            bHandler->addBullet(Bullet::Ptr(
                new Bullet(spread_angles[1], shot_speed_ - speed_d, x, y, vx,
                           vy, renderEngine_, physicsEngine_, (Entity*)physicsOwner_)));
            bHandler->addBullet(Bullet::Ptr(
                new Bullet(spread_angles[2], shot_speed_ - speed_d, x, y, vx,
                           vy, renderEngine_, physicsEngine_, (Entity*)physicsOwner_)));

            // Flash
            createParticles(paHandler, spread_angles[0], x, y);
            createParticles(paHandler, spread_angles[1], x, y);
            createParticles(paHandler, spread_angles[2], x, y);

            last_shot_start_ticks_ = SDL_GetTicks();
            shots_in_magazine_--;
            if (shots_in_magazine_ == 0) {
                reload();
            }
        }
    }
}

// Spread helper function for calculating rads
std::vector<double> DoubleShotgun::calc_spread_angles(double angle_rad,
                                                      bool alternate_gun) {
    // First shot spread in rad
    double twist = 0.1f;
    if (alternate_gun) {
        twist = -twist;
    }
    std::vector<double> three_angles = {angle_rad, angle_rad - twist,
                                        angle_rad - 2 * twist};
    return three_angles;
}
