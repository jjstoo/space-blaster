//
// Created by joni on 5.1.2021.
//

#ifndef BLASTER_CAMPERPUNISHER_H
#define BLASTER_CAMPERPUNISHER_H
#include "ship.h"
#include "asteroidHandler.h"
#include "timingtask.h"
#include "coordinateutils.h"

class CamperPunisher {
  public:
    typedef std::shared_ptr<CamperPunisher> Ptr;
    CamperPunisher(Ship::Ptr ship, AsteroidHandler::Ptr asteroidHandler);
    void update();

  private:
    const static unsigned int area_r_ = 300;
    const static unsigned int delay_s_ = 5;
    const static unsigned int delay_ms_ = delay_s_ * 1000;
    const static unsigned int update_delay_ms_ = 500;
    SDL_Point pos_;
    TimingTask punishTimer;
    TimingTask updateTimer;
    Ship::Ptr ship_;
    AsteroidHandler::Ptr asteroidHandler_;

    void releaseAsteroid_();

};

#endif // BLASTER_CAMPERPUNISHER_H
