#ifndef ASTEROIDHANDLER_H
#define ASTEROIDHANDLER_H

#include "../physics/physicsengine.h"
#include "asteroid.h"
#include "particleHandler.h"
#include "timingtask.h"

#include <map>
#include <memory>

class AsteroidHandler {
  public:
    typedef std::shared_ptr<AsteroidHandler> Ptr;
    static const unsigned int DEFAULT_SPAWN_INTERVAL = 1000;
    static const unsigned int MINIMUM_SPAWN_INTERVAL = 10;
    static const unsigned int MINIMUM_ASTEROID_SIZE = 10;

    AsteroidHandler(PhysicsEngine *physicsEngine_,
                    RenderEngine *renderEngine,
                    ParticleHandler *particleHandler);
    ~AsteroidHandler();

    ///
    /// \brief Runs update tasks on the currently active asteroids
    ///
    void update();

    ///
    /// \brief Spawns a new asteroid to most optimal position
    ///
    void spawnAsteroid();

    /// Creates an asteroid to given position and target direction
    /// \param pos spawn position
    /// \param direction asteroid direction
    /// \param speed_factor speed factor for automated speed calculation
    void createAsteroid(SDL_Point pos, int direction, double speed_factor=1.0);

    ///
    /// \brief Reset all current asteroids
    ///
    void resetAsteroids();

    ///
    /// \brief Sets asteroid spawn interval
    /// \param time spawn interval in ms
    ///
    void setSpawnInterval(unsigned int time);

    ///
    /// \brief Sets asteroid spawn speed (=velocity)
    /// \param speed spawn speed
    ///
    void setSpawnSpeed(unsigned int speed);

    std::vector<Asteroid::Ptr> asteroids;

  private:

    ///
    /// \brief Checks for most optimal spawn position
    /// \return best screen quarter to spawn in
    ///
    ScreenPosition getBestSpawn_();

    ///
    /// \brief Returns a random destination point on a screen quarter
    /// \param pos scren quarter
    /// \return destination
    ///
    SDL_Point endPointFromQuarter_(ScreenPosition pos);

    ///
    /// \brief Returns a random side of the play area
    /// \param dismissed side to dismiss
    /// \return random side index
    ///
    int randomSide_(int dismissed=-1);

    ///
    /// \brief Returns a random spawn point on a screen quarter
    /// \param pos screen quarter
    /// \param off_screen buffer to get a spawn point outside of screen
    /// \return random spawn point
    ///
    SDL_Point randomPointOnQuarter_(ScreenPosition pos, int off_screen = 100);

    ///
    /// \brief Splits an asteroid to two smaller pieces
    /// \param asteroid Asteroid to split
    ///
    void splitAsteroid_(Asteroid *asteroid);

    ///
    /// \brief Runs update tasks on asteroids
    ///
    void updateAsteroids_();

    unsigned int spawntime_;
    unsigned int spawnspeed_;

    PhysicsEngine *physicsEngine_;
    RenderEngine *renderEngine_;
    ParticleHandler *particleHandler_;
    TimingTask spawnTimer_;


};

#endif
