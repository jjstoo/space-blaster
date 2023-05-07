#ifndef BULLETHANDLER_H
#define BULLETHANDLER_H

#include "weapons/bullet.h"
#include <set>

class BulletHandler {
  public:
    BulletHandler();
    ~BulletHandler();

    ///
    /// \brief Run update tasks on bullets and delete bullets out-of-bounds
    ///
    void update();

    ///
    /// \brief Add a bullet to the handler
    /// \param bullet Bullet instance
    ///
    void addBullet(Bullet::Ptr bullet);

    ///
    /// \brief Get currently active bullets
    /// \return set of bullets
    ///
    std::set<Bullet::Ptr> getBullets();

  private:
    // Data structure for storing the shot bullets
    std::set<Bullet::Ptr> bullets_;
};

#endif // BULLETHANDLER_H
