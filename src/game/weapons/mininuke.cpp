#include "mininuke.h"

void MiniNuke::shoot(BulletHandler *bHandler, ParticleHandler *paHandler,
                     double /*direction_rad*/, int x, int y) {
    if ((!isReloading()) && (getShotsLeft() > 0)) {

        if ((SDL_GetTicks() - last_shot_start_ticks_) > fire_rate_cooldown_) {
            int nuke_particles = 200;
            for (int i = 0; i < nuke_particles; i++) {
                double angle = (double)i / nuke_particles * 2 * PI;
                double speed = shot_speed_;
                if (i % 2)
                    speed *= 0.9; // every other nuke particle has slighty
                                  // slower speed
                if (i % 3)
                    speed *= 0.8; // every third is even slower because it looks
                                  // even cooler
                bHandler->addBullet(Bullet::Ptr(new Bullet(
                    angle, speed, x, y, physicsOwner_->getVelX(),
                               physicsOwner_->getVelY(),
                    renderEngine_, physicsEngine_, (Entity*)physicsOwner_)));
                createParticles(paHandler, angle, x, y);
            }

            // Update weapon status
            last_shot_start_ticks_ = SDL_GetTicks();
            shots_in_magazine_--;
            if (shots_in_magazine_ == 0) {
                reload();
            }
        }
    }
}
