#include "particleHandler.h"

#include <memory>
#include "rng.h"
#include "../game.h"

ParticleHandler::ParticleHandler(PhysicsEngine *physicsEngine,
                                 RenderEngine *renderEngine)
    : physicsEngine_(physicsEngine), renderEngine_(renderEngine) {}

void ParticleHandler::update() {
    // Delete dead particles
    std::vector<Particle::Ptr> particles_new;
    particles_new.reserve(particles.size());
    for (auto & p : particles) {
        p->update();
        if (p->alive)
            particles_new.push_back(p);
    }

    particles.clear();
    particles = particles_new;
}

void ParticleHandler::createParticle(double direction_rad, double launch_speed,
                                     double vx, double vy, int x, int y,
                                     int max_lifespan, SDL_Color color,
                                     ParticleSize size) {

    // Calculate initial direction and speed based on launch speed and launching
    // party movement
    double px = launch_speed * cos(direction_rad);
    double py = launch_speed * sin(direction_rad);

    px += vx;
    py += vy;

    double speed = sqrt(pow(px, 2) + pow(py, 2));
    double theta = atan2(py, px);

    int ttl = random_int_in_range<int>(1, max_lifespan);

    particles.push_back(std::make_shared<Particle>(physicsEngine_, renderEngine_, theta, speed, x, y, ttl, color, size));

}

void ParticleHandler::resetParticles() { particles.clear(); }

void ParticleHandler::createParticleBurst(double heading, double speed, int x,
                                          int y, double vx, double vy,
                                          int amount, double spread,
                                          int max_lifespan, SDL_Color color,
                                          bool normal) {
    double heading_lo = heading - (spread / 2);
    double heading_hi = heading + (spread / 2);
    double heading_r;
    for (int j = 1; j <= amount; j++) {
        if (normal)
            heading_r = random_float_in_range(heading_lo, heading_hi,
                                              Distribution::NORMAL);
        else
            heading_r = random_float_in_range(heading_lo, heading_hi,
                                              Distribution::UNIFORM);

        double speed_r = random_normal_float(speed, speed / 4);
        if (random_int_in_range<int>(0, 2) > 1) {
            createParticle(heading_r, speed_r, vx, vy, x, y, max_lifespan,
                           color, ParticleSize::PARTICLE_M);
        } else {
            createParticle(heading_r, speed_r, vx, vy, x, y, max_lifespan,
                           color, ParticleSize::PARTICLE_S);
        }
    }
}

void ParticleHandler::createParticleExplosion(int x, int y, double speed,
                                              double vx, double vy, int amount,
                                              int max_lifespan,
                                              SDL_Color color) {
    for (int i = 0; i < amount; ++i) {
        double angle = 2 * PI * (static_cast<double>(i) / amount);
        double random_speed_factor = random_float_in_range(0.1, 1.9, NORMAL);
        if (random_int_in_range<int>(0, 2) > 1)
            createParticle(angle, speed * random_speed_factor, vx, vy, x, y,
                           max_lifespan, color, ParticleSize::PARTICLE_M);
        else
            createParticle(angle, speed * random_speed_factor, vx, vy, x, y,
                           max_lifespan, color, ParticleSize::PARTICLE_S);
    }
}
