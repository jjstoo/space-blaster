#ifndef WEAPON_H
#define WEAPON_H

#include "bullet.h"
#include <memory>
#include <string>

#include "../../blaster.h"
#include "../bullethandler.h"
#include "../particleHandler.h"
#include "SDL2/SDL_mutex.h"

class Weapon {
  public:
    typedef std::shared_ptr<Weapon> Ptr;

    Weapon(RenderEngine *renderEngine,
           PhysicsEngine *physicsEngine,
           Entity *owner);
    virtual ~Weapon();

    ///
    /// \brief Runs the shooting sequence for the weapon.
    /// This function MUST call isReloading() to update the
    /// reload status before shooting.
    /// \param bHandler BulletHandler for storing the shot bullets
    /// \param paHandler ParticleHandler for storing wandering particles
    /// \param x shot start coordinate x
    /// \param y shot start coordinate y
    ///
    virtual void shoot(BulletHandler *bHandler, ParticleHandler *paHandler,
                       double direction_rad, int x, int y);

    ///
    /// \brief Get shots left in magazine
    /// \return number of shots
    ///
    int getShotsLeft();

    ///
    /// \brief Runs the reload sequence for the weapon
    ///
    void reload();

    ///
    /// \brief Updates and returns current reload status
    /// \return true if the weapon is reloading
    ///
    bool isReloading();

    ///
    /// \brief Stop the reload task thread safely
    ///
    void stopReloading();

    ///
    /// \brief Get name of the weapon
    /// \return weapon name
    ///
    std::string getName();

    ///
    /// \brief Resets weapon state
    ///
    void reset();

  protected:
    ///
    /// \brief Reload handling run in a separate thread. Waits for
    /// reload_cooldown_ amount of ticks.
    /// \param ptr to weapon instance
    /// \return nothing
    ///
    static int reloadTask(void *ptr);

    ///
    /// \brief Creates particles when firing the weapon
    /// \param x Particle start cordinate x
    /// \param y Particle start cordinate y
    ///
    virtual void createParticles(ParticleHandler *paHandler,
                                 double direction_rad, int x, int y);

    RenderEngine *renderEngine_;
    PhysicsEngine *physicsEngine_;
    PhysicsObject *physicsOwner_;
    Entity* owner_;

    bool reloading_;
    bool reload_stop_request_ = false;
    SDL_mutex *mtx_;

    Uint32 reload_start_ticks_;
    Uint32 last_shot_start_ticks_;

    std::string weapon_name_;
    int magazine_size_;
    int shots_in_magazine_;
    unsigned int reload_cooldown_;
    unsigned int fire_rate_cooldown_;
    double shot_speed_;
};
#endif // WEAPON_H
