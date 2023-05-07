#ifndef PARTICLE_H
#define PARTICLE_H

#include "../physics/physicsengine.h"
#include "../physics/physicsobject.h"
#include "../rendering/renderobject.h"
#include "SDL2/SDL.h"
#include "graphics.h"
#include <memory>

// TODO cleanup and make particle volatility FPS independent

enum ParticleSize { PARTICLE_S, PARTICLE_M };

///
/// \brief The Particle class
///
class Particle : public PhysicsObject {
  public:
    typedef std::shared_ptr<Particle> Ptr;

  private:
    // Size of individual particle, shape defined in cpp
    static const int pixels_m = 5;
    double direction_rad_;
    int max_ttl_;
    // Time to live, used also in rendering for determining particles color
    int ttl_;

    Uint32 frame_start_ = 0;
    Uint8 r_;
    Uint8 g_;
    Uint8 b_;
    ParticleSize size_;
    Polygon body_;

    std::vector<SDL_Point> getShape_(ParticleSize size, int x, int y);

  public:
    ///
    /// \brief Represents an individual particle
    /// \param physicsEngine PhysicsEngine instance
    /// \param renderEngine RenderEngine instance
    /// \param direction_initial launch direction in radians
    /// \param v_initial initial speed
    /// \param x_initial initialx-Coordinate
    /// \param y_initial initial y-Cooridnate
    /// \param max_ttl maximum lifetime of the particle
    ///
    Particle(PhysicsEngine *physicsEngine, RenderEngine *renderEngine,
             double direction_initial, double v_initial, double x_initial,
             double y_initial, int max_ttl,
             SDL_Color color = SDL_Color{230, 185, 20, 255},
             ParticleSize size = ParticleSize::PARTICLE_M);

    ///
    /// \brief Update particle position and status
    ///
    void update();

    // How often in (0-100) a particle should gain random momentum
    int volatility_ = 0;
    bool alive;
};

#endif // PARTICLE_H
