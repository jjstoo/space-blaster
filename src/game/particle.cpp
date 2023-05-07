#include "particle.h"
#include "../game.h"
#include "graphics.h"

#ifdef _WIN32
#include <math.h>
#endif

Particle::Particle(PhysicsEngine *physicsEngine,
                   RenderEngine *renderEngine,
                   double direction_initial,
                   double v_initial,
                   double x_initial,
                   double y_initial,
                   int max_ttl,
                   SDL_Color color,
                   ParticleSize size)
    : PhysicsObject(physicsEngine, 0.5, x_initial, y_initial, true) {
    // Storing constructor variables
    max_ttl_ = max_ttl;
    ttl_ = max_ttl_;
    direction_rad_ = direction_initial;
    alive = true;

    r_ = color.r;
    g_ = color.g;
    b_ = color.b;

    size_ = size;
    body_.setRenderLayer(TOP_RENDER_LAYER_IDX-2);
    body_.init(renderEngine, getShape_(size, x_initial, y_initial),
               x_initial, y_initial);
    body_.setColor(SDL_Color{r_, g_, b_, 0xff});
    body_.setRenderType(POINT);

    // Letting physics class handle movement
    setSpeed(direction_rad_, v_initial);
    frame_start_ = SDL_GetTicks();
}

std::vector<SDL_Point> Particle::getShape_(ParticleSize size, int x, int y) {
    std::vector<SDL_Point> shape;
    if (size == PARTICLE_M) {
        shape = {{x, y - 1}, {x - 1, y}, {x + 1, y}, {x, y + 1}, {x, y}};
    } else if (size == PARTICLE_S) {
        shape = {{x, y}};
    }

    return shape;
}

/// Render function
void Particle::update() {
    // Record timescale
    Uint32 now = SDL_GetTicks();
    Uint32 delta = now - frame_start_;
    frame_start_ = now;

    // Move body
    body_.moveAbsolute(getPosX(), getPosY());

    // This part calculates some arbitary movement during lifespan of a particle
    int chance_ = rand() % 100 + 1;
    if (chance_ < volatility_) {
        double direction = static_cast<double>(rand() % 10) / 10.0f * 2 * PI;
        double magnitude =
            static_cast<double>(rand() % 10) / 10.0f *
            static_cast<double>(0.0015); // TODO make this timescale tolerant
        // Updating deviations in movement
        addForce(direction, magnitude);
        // Reducing lifespan of deviating particles to prevent lucky, super
        // fast, particles
        ttl_ = ttl_ - 1;
    }

    // Determining a varying color for a particle, depending on its current ttl
    // TODO make this use parameters such as set_color(<color_1>, <color_2>)
    // In this case Color_1 is (190, 135, 90) and
    // Color_2 is (230, 185, 20)
    // TODO Replace this logic with ttl_left, which would be more intuitive
    double ttl_spent = 1 - (ttl_ / max_ttl_);

    // Particle should break down after 80ms
    if ((max_ttl_ - ttl_) > 80)
        size_ = PARTICLE_S;

    Uint8 cR = static_cast<Uint8>(r_ - (40 * ttl_spent)); // colorValue
    Uint8 cG = static_cast<Uint8>(g_ - (50 * ttl_spent)); // colorValue
    Uint8 cB = static_cast<Uint8>(b_ + (70 * ttl_spent)); // colorValue
    // Making "old" particles transparent
    Uint8 cV = 0xFF;
    if (ttl_spent < 0.9f) {
        cV = 0x8F;
    }

    body_.setColor(SDL_Color{cR, cG, cB, cV});

    if (body_.outOfBounds()) {
        alive = false;
    }

    // Count down particle lifespan
    ttl_ = ttl_ - static_cast<int>(delta); // TODO make this timescale tolerant
    if (ttl_ < 2) {
        alive = false;
    }
}
