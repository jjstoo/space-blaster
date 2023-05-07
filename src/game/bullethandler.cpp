#include "bullethandler.h"
#include "../game.h"

BulletHandler::BulletHandler() {}

BulletHandler::~BulletHandler() { bullets_.clear(); }

void BulletHandler::update() {
    // Delete dead particles
    auto end = bullets_.end();
    for (auto it = bullets_.begin(); it != end;) {
        it->get()->update();
        if (!it->get()->alive)
            it = bullets_.erase(it);
        else
            ++it;
    }
}

void BulletHandler::addBullet(Bullet::Ptr bullet) { bullets_.insert(bullet); }

std::set<Bullet::Ptr> BulletHandler::getBullets() { return bullets_; }
