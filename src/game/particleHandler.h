#ifndef PARTICLEHANDLER_H
#define PARTICLEHANDLER_H

#include "particle.h"
#include <memory>
#include <vector>

static const SDL_Color DEFAULT_PARTICLE_COLOR = SDL_Color{230, 185, 20, 255};
static const double DEFAULT_EXPLOSION_SPEED = 0.5f;
static const int DEFAULT_PARTICLE_LIFESPAN = 200;

class PhysicsEngine;
class RenderEngine;
class ParticleHandler {
  public:
    typedef std::shared_ptr<ParticleHandler> Ptr;
    ParticleHandler(PhysicsEngine *physicsEngine, RenderEngine *renderEngine);

    ///
    /// \brief Updates and deletes particles that are no longer alive
    ///
    void update();

    ///
    /// \brief Creates a single particle with the given
    /// location and speed
    /// \param direction_rad particle direction
    /// \param launch_speed particle launch speed
    /// \param vx launcher initial speed in x direction
    /// \param vy launcher initial speed in y direction
    /// \param x launch coordinate x
    /// \param y launch coordinate y
    /// \param max_lifespan maximum lifespan of the particle
    ///
    void createParticle(double direction_rad, double launch_speed, double vx,
                        double vy, int x, int y, int max_lifespan,
                        SDL_Color color = DEFAULT_PARTICLE_COLOR,
                        ParticleSize size = ParticleSize::PARTICLE_M);

    ///
    /// \brief Creates a burst of particles
    /// \param heading particle direction
    /// \param speed particle launch speed
    /// \param x launch coordinate x
    /// \param y launch coordinate y
    /// \param vx launcher initial speed in x direction
    /// \param vy launcher initial speed in y direction
    /// \param amount number of particles
    /// \param spread spread angle
    /// \param max_lifespan maximum lifespan of particles
    /// \param normal sets the spread to be normally distributed
    ///
    void createParticleBurst(double heading, double speed, int x, int y,
                             double vx, double vy, int amount, double spread,
                             int max_lifespan,
                             SDL_Color color = DEFAULT_PARTICLE_COLOR,
                             bool normal = true);

    ///
    /// \brief Creates an explosion of particles
    /// \param x launch coordinate x
    /// \param y launch coordinate y
    /// \param speed launch speed
    /// \param vx launcher initial speed in x direction
    /// \param vy launcher initial speed in y direction
    /// \param amount number of particles
    /// \param max_lifespan maximum lifespan of particles
    /// \param color particle color
    ///
    void createParticleExplosion(int x, int y, double speed, double vx,
                                 double vy, int amount, int max_lifespan,
                                 SDL_Color color = DEFAULT_PARTICLE_COLOR);

    ///
    /// \brief resetParticle deletes all particles
    ///
    void resetParticles();

    // Data structure for storing particles
    std::vector<Particle::Ptr> particles;

  private:
    PhysicsEngine *physicsEngine_ = nullptr;
    RenderEngine *renderEngine_ = nullptr;
};

#endif // PARTICLEHANDLER_H
