#include "asteroid.h"
#include "rng.h"
#include "../game.h"

#ifdef _WIN32
#include <math.h>
#endif

Asteroid::~Asteroid() = default;
Asteroid::Asteroid(PhysicsEngine *physicsEngine,
                   RenderEngine *renderEngine,
                   int x_initial,
                   int y_initial,
                   int initial_angle,
                   double initial_v,
                   unsigned int initial_corners,
                   unsigned int initial_size) :
    PhysicsObject(physicsEngine,
                    10,
                    x_initial,
                    y_initial,
                    true,
                    initial_v,
                    false),
    Entity(false, ASTEROID) {

    // Set collision properties
    setCollidable(true);
    ENABLE_COLLISION(BULLET);
    ENABLE_COLLISION(SHIP);
    ENABLE_COLLISION(ASTEROID);

    renderEngine_ = renderEngine;
    // Set initial values
    alive_ = true;
    n_collisions = 0;

    // Size and initial movement direction
    size = initial_size;
    angle = initial_angle;
    max_r_ = static_cast<int>(size);
    min_r_ = static_cast<int>(size - size * RADIAL_VARIATION);
    direction_rad_ = static_cast<double>(((initial_angle)*PI) / 180);

    // Set physics properties
    setSpeed(direction_rad_, initial_v);
    setRadius(max_r_);

    // Shape and screen position
    body.init(renderEngine, createShape_(static_cast<int>(initial_corners), x_initial, y_initial),
              x_initial, y_initial);
    body.setRenderType(FILL);
    body.setColor(SDL_Color{36, 248, 229, 255});
    screenPos = body.getScreenPosition();
}

std::vector<SDL_Point> Asteroid::createShape_(int corners, int x, int y) const {
    SDL_Point first_point;
    std::vector<SDL_Point> outline;

    // Generate random radius for each of the corners
    for (int j = 0; j < corners; j++) {
        // Randomize radius
        int r = random_int_in_range<int>(min_r_, max_r_);

        // Calculate angle for the radius
        auto angle =
            static_cast<double>((((j * (360 / corners - 1)) + 90) * PI) / 180);

        // Get coordinates
        int x_r = static_cast<int>(r * cos(angle) + x);
        int y_r = static_cast<int>(r * sin(angle) + y);

        // Generate point and save
        SDL_Point point_r = {x_r, y_r};
        outline.push_back(point_r);

        if (j == 0)
            first_point = point_r;
    }

    // close the loop
    outline.push_back(first_point);

    return outline;
}

/// Renderer
void Asteroid::update() {
    // Update body position
    body.moveAbsolute(getPosX(), getPosY());
    screenPos = body.getScreenPosition();

#if DEBUG_ASTEROID_BORDERS
    int X = body->getMaxX();

    SDL_SetRenderDrawColor(Game::RENDERER, 255,0,0,255);
    SDL_RenderDrawLine(Game::RENDERER, X, 0, X, 7000);
#endif

    // Check if asteroid is out of bounds
    // TODO use global values instead of hardcoded 50/-50
    if (body.outOfBounds(300)) {
        alive_ = false;
    }
}

void Asteroid::markDead() { alive_ = false; }
void Asteroid::markSplit() { split_ = true; }
bool Asteroid::isAlive() { return alive_; }
bool Asteroid::isDueSplit() { return split_; }

void Asteroid::handleAsteroidCollision_(Asteroid *a2) {
    double i = physicsCollisionWith(a2);
    if (i != 0.0)
        n_collisions++;

    double i_factor = i / getMass();
    if (i_factor > BREAK_TRESHOLD) {
        markDead();
    } else if (i_factor > CRUMBLE_TRESHOLD) {
        markDead();
        markSplit();
    }
}

void Asteroid::collisionWith(Entity *e) {
    switch (e->getType()) {
    case SHIP:
        markDead();
        break;
    case ASTEROID:
        handleAsteroidCollision_((Asteroid *)e);
        break;

    case BULLET:
        markDead();
        markSplit();
        break;
    }
}

Polygon *Asteroid::getBody() { return &body; }

