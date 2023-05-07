#ifndef SHIP_H
#define SHIP_H

#include "../physics/physicsengine.h"
#include "../physics/physicsobject.h"
#include "../rendering/renderobject.h"
#include "bullethandler.h"
#include "graphics.h"
#include "particleHandler.h"
#include "textEngine.h"
#include "viewport.h"
#include "entity.h"

#include "weapons/cannon.h"
#include "weapons/doubleshotgun.h"
#include "weapons/machineGun.h"
#include "weapons/mininuke.h"
#include "weapons/weapon.h"

#define SHIPLIVES 3
#define THRUST_FORCE 0.02
#define SHIP_MAX_SPEED 2.0
#define ROTATION_SPEED 5

///
/// \brief The Ship class
///
/// This class represents the player-controlled main vessel.
/// Weapons are handled within this class instance as weapons are seen as owned
/// by the ship.
///
class Ship : public PhysicsObject, public Entity {
  public:
    typedef std::shared_ptr<Ship> Ptr;

    enum Rotation { Clockwise, Anticlockwise };

  public:
    // Ship status
    bool alive;

    // Ship outline
    Polygon body;
    int corners = 6;

    /// Functions

    ///
    /// \brief Constructor for the main vessel
    /// \param physicsEngine pointer to PhysicsEngine instance
    /// \param renderEngine pointer to RenderEngine instance
    /// \param particleHandler pointer to ParticleHandler instance
    /// \param x_initial initial x coordinate
    /// \param y_initial initial y coordinate
    /// \param initial_angle initial orientation
    /// \param h height of the chassis
    /// \param w width of the chassis
    ///
    Ship(PhysicsEngine *physicsEngine, RenderEngine *renderEngine,
         ParticleHandler *particleHandler, int x_initial, int y_initial,
         double initial_angle, int h, int w);
    ~Ship();

    ///
    /// \brief Rotate ship to clockwise or anticlockwise
    /// \param direction Direction to rotate the ship to
    ///
    void rotate(Rotation direction);

    ///
    /// \brief Use ships thruster to accelerate the ship
    ///
    void accelerate();

    ///
    /// \brief Fire the ships current weapon
    /// \param bHandler bullethandler instance
    ///
    void shoot(BulletHandler *bHandler);

    ///
    /// \brief Switch current weapon to a given slot
    /// \param slot weapon to switch to
    ///
    void switchWeapon(unsigned long slot);

    ///
    /// \brief Reload ships weapon
    ///
    void reload();

    ///
    /// \brief Launch collision cycle
    ///
    void collision();

    ///
    /// \brief Reset ship status
    /// \param x_initial initial x coordinate
    /// \param y_initial initial y coordinate
    /// \param initial_angle initial orientation
    ///
    void reset(int x_initial, int y_initial, double initial_angle);

    ///
    /// \brief Get ships current time alive
    /// \return time alive
    ///
    unsigned int getTimeAlive();

    /// Get currently active weapon
    /// \return Pointer to active weapon
    Weapon* getActiveWeapon();

    /// Get lives left
    /// \return lives
    int getLives();

    ///
    /// \brief Run update tasks for the ship
    ///
    void update();

    Polygon *getBody() override;
    void collisionWith(Entity *e) override;

  private: // functions
    std::vector<std::string> getOutline_();
    void updateTimeAlive_();
    std::vector<SDL_Point> calculateChassis_(double x, double y);
    void createParticles_();

  private: // variables

    // Ship status
    unsigned int lives_;
    bool damage_taken_ = false;

  public:
    bool isDamageTaken() const;
  private:
    // Used for calculating time alive
    unsigned int time_alive_ = 0;
    unsigned int time_alive_change_ = 0;

    // Chassis positioning
    double heading_rad_;
    int height_;
    int width_;
    int top_corner_idx_;
    int bottom_left_corner_idx_;
    int bottom_right_corner_idx_;

    // Spawn cooldown to avoid accidental shooting
    Uint32 spawn_cooldown_ = 0;

    // Ship particle speed and particlehandler instance
    ParticleHandler *particleHandler_;
    const double particleSpeed_ = 0.5;

    // Renderengine instance
    RenderEngine *renderEngine_;

    // Weapons
    std::vector<Weapon::Ptr> weapons_;
    Weapon::Ptr activeWeapon_;
};

#endif
