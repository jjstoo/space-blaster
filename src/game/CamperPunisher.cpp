//
// Created by joni on 5.1.2021.
//

#include "CamperPunisher.h"

#include <utility>

CamperPunisher::CamperPunisher(Ship::Ptr ship,
                               AsteroidHandler::Ptr asteroidHandler)
    : punishTimer(delay_ms_, true),
    updateTimer(update_delay_ms_, true),
    ship_(std::move(ship)),
    asteroidHandler_(std::move(asteroidHandler)) {
    pos_.x = 0;
    pos_.y = 0;
    punishTimer.start();
    updateTimer.start();
}

void CamperPunisher::update() {
    if (updateTimer.isDue()) {
        // Current ship position
        SDL_Point p{static_cast<int>(ship_->getPosX()),
                    static_cast<int>(ship_->getPosY())};
        // Compare to previous position
        if (CoordinateUtils::distance(p, pos_) > area_r_) {
            // Player has left the area, reset center point position
            // for next iteration
            pos_ = p;
            punishTimer.start();
        } else if (punishTimer.isDue()) {
            // Player hasn't left the area during the timer
            // Punish with precision targeted asteroid
            releaseAsteroid_();
        }
    }
}

void CamperPunisher::releaseAsteroid_() {
    const static int delta = 100;
    SDL_Point launch_position;
    switch (ship_->getBody()->getScreenPosition()) {
    case NW:
        launch_position.x = -delta;
        launch_position.y = -delta;
        break;
    case NE:
        launch_position.x = GAME_AREA_WIDTH + delta;
        launch_position.y = -delta;
        break;
    case SE:
        launch_position.x = GAME_AREA_WIDTH + delta;
        launch_position.y = GAME_AREA_HEIGHT + delta;
        break;
    case SW:
        launch_position.x = -delta;
        launch_position.y = GAME_AREA_HEIGHT + delta;
    }

    SDL_Point p{static_cast<int>(ship_->getPosX()),
                static_cast<int>(ship_->getPosY())};

    asteroidHandler_->createAsteroid(launch_position, static_cast<int>(
        CoordinateUtils::angle_between(launch_position, p) * RAD2DEG), 3.0);
}

