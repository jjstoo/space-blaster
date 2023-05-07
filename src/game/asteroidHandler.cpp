#include "asteroidHandler.h"
#include "coordinateutils.h"
#include "rng.h"
#include "../blaster.h"
#include "../game.h"

#include <iostream>

#ifdef _WIN32
#include <math.h>
#endif

/// Destructor
AsteroidHandler::~AsteroidHandler() { resetAsteroids(); }

/// Initializer
AsteroidHandler::AsteroidHandler(PhysicsEngine *physicsEngine,
                                 RenderEngine *renderEngine,
                                 ParticleHandler *particleHandler)
    : renderEngine_(renderEngine), particleHandler_(particleHandler),
      spawnTimer_(AsteroidHandler::DEFAULT_SPAWN_INTERVAL),
      physicsEngine_(physicsEngine) {
    spawntime_ = 20;
    spawnspeed_ = 3;
    spawnTimer_.start();

#if DEBUG_ONE_ASTEROID
    asteroids.push_back(Asteroid::Ptr(
        new Asteroid(physicsEngine, renderEngine, 800, 500, 0, 0, 16, 200)));
#elif DEBUG_TWO_ASTEROID_COLLISION
    asteroids.push_back(Asteroid::Ptr(
        new Asteroid(physicsEngine, renderEngine, 400, 500, 10, 0.4, 8, 50)));
    asteroids.push_back(Asteroid::Ptr(new Asteroid(
        physicsEngine, renderEngine, 1600, 700, 190, 0.4, 8, 50)));
#endif
}

/// Returns random int 0-3 for selecting a side of the game area
/// If a parameter is given, this function dismisses the given number
int AsteroidHandler::randomSide_(int dismissed) {
    int side = 0;
    bool found = false;
    while (!found) {
        side = rand() % 4;
        if (side != dismissed) {
            found = true;
        }
    }
    return side;
}

SDL_Point AsteroidHandler::randomPointOnQuarter_(ScreenPosition pos,
                                                 int off_screen) {

    SDL_Point point;
    int x = 0;
    int y = 0;
    int side = rand() % 2; // which side of the corner is used

    switch (pos) {
    case NE:
        if (side) {
            // From top, right corner
            x = (rand() % (GAME_AREA_WIDTH / 2 + 1)) + GAME_AREA_WIDTH / 2;
            y = -off_screen;
        } else {
            // From right, top corner
            x = GAME_AREA_WIDTH + off_screen;
            y = rand() % (GAME_AREA_HEIGHT / 2 + 1);
        }
        break;
    case NW:
        if (side) {
            // From top, left corner
            x = rand() % (GAME_AREA_WIDTH / 2 + 1);
            y = -off_screen;
        } else {
            // From left, top corner
            x = -off_screen;
            y = rand() % (GAME_AREA_HEIGHT / 2 + 1);
        }
        break;
    case SE:
        if (side) {
            // From bottom, right corner
            x = (rand() % (GAME_AREA_WIDTH / 2 + 1)) + GAME_AREA_WIDTH / 2;
            y = GAME_AREA_HEIGHT + off_screen;
        } else {
            // From right, bottom corner
            x = GAME_AREA_WIDTH + off_screen;
            y = (rand() % (GAME_AREA_HEIGHT / 2 + 1)) + GAME_AREA_WIDTH / 2;
        }
        break;
    case SW:
        if (side) {
            // From bottom, left corner
            x = rand() % (GAME_AREA_WIDTH / 2 + 1);
            y = GAME_AREA_HEIGHT + off_screen;
        } else {
            // From left, bottom corner
            x = -off_screen;
            y = (rand() % (GAME_AREA_HEIGHT / 2 + 1)) + GAME_AREA_WIDTH / 2;
        }
        break;
    }
    point.x = x;
    point.y = y;

    return point;
}

ScreenPosition AsteroidHandler::getBestSpawn_() {
    // Count asteroids in each screen quarter
    int ne = 0, nw = 0, se = 0, sw = 0;
    for (const auto& a : asteroids) {
        switch (a->screenPos) {
        case NE:
            ne++;
            break;
        case NW:
            nw++;
            break;
        case SE:
            se++;
            break;
        case SW:
            sw++;
            break;
        }
    }

    // Find and return the best quarter
    int best = ne;
    ScreenPosition ret = NE;
    if (nw < best) {
        best = nw;
        ret = NW;
    }
    if (se < best) {
        best = se;
        ret = SE;
    }
    if (sw < best) {
        best = sw;
        ret = SW;
    }
    return ret;
}

SDL_Point AsteroidHandler::endPointFromQuarter_(ScreenPosition pos) {
    // Generate a valid end point for a point in a screen quarter
    switch (pos) {
    case NE:
        return randomPointOnQuarter_(SW, 0);
    case NW:
        return randomPointOnQuarter_(SE, 0);
    case SE:
        return randomPointOnQuarter_(NW, 0);
    case SW:
        return randomPointOnQuarter_(NE, 0);
    default:
        return randomPointOnQuarter_(NW, 0);
    }
}

void AsteroidHandler::splitAsteroid_(Asteroid *asteroid) {
    // Break asteroid to smaller pieces
    if (asteroid->size > MINIMUM_ASTEROID_SIZE) {

        int x = asteroid->body.x;
        int y = asteroid->body.y;
        int angle = static_cast<int>(asteroid->getVelAngle() * RAD2DEG) % 360;
        int angle_a;
        int angle_b;

        if (angle > 0 && angle < 180) {
            angle_a = (angle + 15);
            angle_b = (angle - 15);
        } else {
            angle_b = (angle + 15);
            angle_a = (angle - 15);
        }

        double v = asteroid->getVelMag();
        unsigned int new_size = asteroid->size * 0.5;

        // Create new asteroids
        Asteroid::Ptr child1 =
                Asteroid::Ptr(new Asteroid(physicsEngine_, renderEngine_,
                                           x - new_size, y - new_size,
                                           angle_a, v, 8, new_size));
        Asteroid::Ptr child2 =
                Asteroid::Ptr(new Asteroid(physicsEngine_, renderEngine_,
                                           x + new_size, y + new_size,
                                           angle_b, v, 8, new_size));

        asteroids.push_back(child1);
        asteroids.push_back(child2);
    }
}

void AsteroidHandler::updateAsteroids_() {
    // Delete asteroid object if it is dead
    std::vector<Asteroid::Ptr> asteroidsNew;
    for (unsigned int i = 0; i < asteroids.size(); i++) {
        asteroids[i]->update();
        if (!asteroids[i]->isAlive()) {
            // Split asteroid to smaller pieces
            if (asteroids[i]->isDueSplit())
                splitAsteroid_(asteroids[i].get());
            // Create explosion
            particleHandler_->createParticleExplosion(
                asteroids[i]->getPosX(), asteroids[i]->getPosY(),
                DEFAULT_EXPLOSION_SPEED, asteroids[i]->getVelX(),
                asteroids[i]->getVelY(), asteroids[i]->size * 10,
                DEFAULT_PARTICLE_LIFESPAN);

        } else {
            asteroidsNew.push_back(asteroids[i]);
        }
    }
    // Update asteroids
    asteroids.clear();
    asteroids = asteroidsNew;
}

void AsteroidHandler::spawnAsteroid() {
    // Starting coordinates
    int x_0;
    int y_0;

    // Ending coordinates
    int x_1;
    int y_1;

    // Find the best screen quarter to spawn, generate a point randomly on the
    // quarter and also generate an end point on the opposing quarter
    ScreenPosition best = getBestSpawn_();
    SDL_Point start = randomPointOnQuarter_(best);
    SDL_Point end = endPointFromQuarter_(best);

    // Calculate asteroid parameters and create instance
    x_0 = start.x;
    y_0 = start.y;
    x_1 = end.x;
    y_1 = end.y;
    int angle = static_cast<int>(
                CoordinateUtils::angle_between(start, end) * RAD2DEG);

    unsigned int speed = random_int_in_range<unsigned int>(1, spawnspeed_);
    unsigned int size = random_int_in_range<unsigned int>(20, 60);

    Asteroid::Ptr newAsteroid(new Asteroid(physicsEngine_, renderEngine_, x_0,
                                           y_0, angle, 0.1 * speed, 8, size));
    asteroids.push_back(newAsteroid);
}

void AsteroidHandler::createAsteroid(SDL_Point pos, int direction,
                                     double speed_factor) {
    auto speed = speed_factor * random_int_in_range<unsigned int>(1, spawnspeed_);
    Asteroid::Ptr a(
        new Asteroid(physicsEngine_, renderEngine_, pos.x, pos.y,
                     direction, 0.1 * speed, 8,
                     random_int_in_range<unsigned int>(20, 60)));

    asteroids.push_back(a);
}

void AsteroidHandler::resetAsteroids() { 
    asteroids.clear(); 
    spawntime_ = 20;
    spawnspeed_ = 3;
    spawnTimer_.setInterval(AsteroidHandler::DEFAULT_SPAWN_INTERVAL);
    spawnTimer_.start();
}

void AsteroidHandler::setSpawnInterval(unsigned int time) {
    if (time > AsteroidHandler::MINIMUM_SPAWN_INTERVAL)
        spawnTimer_.setInterval(time);
    else
        spawnTimer_.setInterval(AsteroidHandler::MINIMUM_SPAWN_INTERVAL);
}

void AsteroidHandler::setSpawnSpeed(unsigned int speed) { spawnspeed_ = speed; }

/// Renderer is called each game tick
void AsteroidHandler::update() {
#if !DEBUG_ONE_ASTEROID && !DEBUG_TWO_ASTEROID_COLLISION && !DEBUG_NO_ASTEROIDS

    if (spawnTimer_.isDue()) {
        spawnAsteroid();
    }

#endif // Spawning enabled

    // Update asteroids/explosions
    updateAsteroids_();
}

