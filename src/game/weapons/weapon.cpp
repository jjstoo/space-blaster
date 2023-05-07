#include "weapon.h"
#include "SDL2/SDL.h"

Weapon::Weapon(RenderEngine *renderEngine, PhysicsEngine *physicsEngine,
               Entity *owner) {
    owner_ = owner;
    physicsOwner_ = (PhysicsObject*) owner;
    renderEngine_ = renderEngine;
    physicsEngine_ = physicsEngine;
    mtx_ = SDL_CreateMutex();
}

Weapon::~Weapon() { SDL_DestroyMutex(mtx_); }

void Weapon::shoot(BulletHandler *bHandler, ParticleHandler *paHandler,
                   double direction_rad, int x, int y) {
    if ((!isReloading()) && (getShotsLeft() > 0)) {
        if ((SDL_GetTicks() - last_shot_start_ticks_) > fire_rate_cooldown_) {
            // variable speed
            double dSpeed = (rand() % 2) * 0.05f;
            bHandler->addBullet(Bullet::Ptr(new Bullet(
                direction_rad, shot_speed_ - dSpeed, x, y,
                           physicsOwner_->getVelX(), physicsOwner_->getVelY(),
                renderEngine_, physicsEngine_, owner_)));
            createParticles(paHandler, direction_rad, x, y);
            last_shot_start_ticks_ = SDL_GetTicks();
            shots_in_magazine_--;

            if (shots_in_magazine_ == 0) {
                reload();
            }
        }
    }
}

void Weapon::createParticles(ParticleHandler *paHandler, double direction_rad,
                             int x, int y) {
    paHandler->createParticleBurst(direction_rad, 0.5f, x, y,
                                   physicsOwner_->getVelX(),
                                   physicsOwner_->getVelY(), 20, 0.05f, 80);
}

void Weapon::reload() {
    bool reloading;
    SDL_LockMutex(mtx_);
    reloading = reloading_;
    SDL_UnlockMutex(mtx_);

    if ((shots_in_magazine_ < magazine_size_) && (!reloading)) {
        SDL_Thread *th = SDL_CreateThread(reloadTask, "reloadThread", this);
        SDL_DetachThread(th);
    }
}

bool Weapon::isReloading() {
    bool ret;
    SDL_LockMutex(mtx_);
    ret = reloading_;
    SDL_UnlockMutex(mtx_);
    return ret;
}

void Weapon::stopReloading() {
    SDL_LockMutex(mtx_);
    if (reloading_)
        reload_stop_request_ = true;
    SDL_UnlockMutex(mtx_);
}

int Weapon::reloadTask(void *ptr) {
    // Now the input argument given to this static thread function is the Weapon
    // instance who created the thread. Therefore, cast the given input pointer
    // to a Weapon class pointer and start the reload task
    Weapon *instance = (Weapon *)ptr;
    instance->reloading_ = true;
    instance->reload_start_ticks_ = SDL_GetTicks();

    // Wake every 100ms to check if the reload "task" is complete, or if the
    // reload should be cancelled
    bool stop_request = false;
    while (((SDL_GetTicks() - instance->reload_start_ticks_) <
            instance->reload_cooldown_) &&
           (!stop_request)) {
        SDL_Delay(100);
        SDL_LockMutex(instance->mtx_);
        stop_request = instance->reload_stop_request_;
        SDL_UnlockMutex(instance->mtx_);
    }

    SDL_LockMutex(instance->mtx_);
    instance->reloading_ = false;
    if (!stop_request) {
        // Only reload if the reload task was not cancelled
        instance->shots_in_magazine_ = instance->magazine_size_;
    }
    instance->reload_stop_request_ = false;
    SDL_UnlockMutex(instance->mtx_);

    return 0;
}

int Weapon::getShotsLeft() {
    int ret;
    SDL_LockMutex(mtx_);
    ret = shots_in_magazine_;
    SDL_UnlockMutex(mtx_);
    return ret;
}

std::string Weapon::getName() { return weapon_name_; }

void Weapon::reset() {
    stopReloading();
    shots_in_magazine_ = magazine_size_;
}
