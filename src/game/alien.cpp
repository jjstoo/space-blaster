#include "alien.h"
#include "../game.h"
#include <math.h>

Alien::Alien(PhysicsEngine *physicsEngine, RenderEngine *renderEngine,
             Ship *player, int x_initial, int y_initial, double initial_angle,
             int h, int w)
    : PhysicsObject(physicsEngine, h, x_initial, y_initial, true,
                    ALIEN_MAX_SPEED),
      direction_rad_(initial_angle), player_(player) {
    body = new Polygon(renderEngine, getChassis_(x_initial, y_initial),
                       x_initial, y_initial);
    body->setColor(SDL_Color{255, 0, 0, 255});
}

Alien::~Alien() {
    if (body)
        delete body;
}

std::vector<SDL_Point> Alien::getChassis_(int x, int y) {
    std::vector<SDL_Point> outline;
    outline.push_back(SDL_Point{x - 10, y - 10});
    outline.push_back(SDL_Point{x + 10, y - 10});
    outline.push_back(SDL_Point{x + 10, y + 10});
    outline.push_back(SDL_Point{x - 10, y + 10});
    outline.push_back(SDL_Point{x - 10, y - 10});
    return outline;
}

void Alien::update() { AIStep_(); }

void Alien::AIStep_() {
    // Calculate distance and angle to player
    double player_x = player_->getPosX();
    double player_y = player_->getPosY();
    double x = getPosX();
    double y = getPosY();

    double dist =
        std::sqrt(std::pow(player_x - x, 2) + std::pow(player_y - y, 2));
    double angle = std::atan2(player_y - y, player_x - x);

    if (dist > 100) {
        double f = dist / 100 * 0.002;
        if (f > 0.01)
            f = 0.02;
        addForceToPoint(player_x, player_y, f);
    } else {
        addForce(angle + PI, (100 - dist) / 100 * 0.02);
    }
}
