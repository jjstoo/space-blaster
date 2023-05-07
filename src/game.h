#ifndef GAME_H
#define GAME_H
class PhysicsObject;
class PhysicsEngine;

#include "blaster.h"
#include "game/alien.h"
#include "game/asteroidHandler.h"
#include "game/background.h"
#include "game/bullethandler.h"
#include "game/particleHandler.h"
#include "game/ship.h"
#include "game/textEngine.h"
#include "game/viewport.h"
#include "game/CamperPunisher.h"
#include "messaging/messagehandler.h"
#include "networking/socket.h"
#include "physics/physicsengine.h"
#include "rendering/renderengine.h"
#include <memory>

///
/// \brief The Game class
///
/// This class is responsible for instantiating and controlling all of the game
/// objects. Keyboard handling is also done within this class.
///
class Game {
  public:
    typedef std::shared_ptr<Game> Ptr;

    Game(bool multiplayer, int port, int host_port, char *host_address);
    ~Game();

    ///
    /// \brief Reads configuration from the given file and
    /// configures the game accordingly
    /// \param conf configuration file name
    ///
    void configure(std::string conf);

    ///
    /// \brief Initializes the game
    ///
    void init();

    ///
    /// \brief Runs the game one tick forward
    ///
    void advance();

    // Current game state
    static GameState gameState;
    static SDL_Window *WINDOW;
    static SDL_Renderer *RENDERER;
    static Viewport *VIEWPORT;

    // Viewport tracking
    Viewport viewport = Viewport(400);

    // Physics engine handling all physics objects
    PhysicsEngine physicsEngine;

    // Render engine handling all renderable objects
    RenderEngine renderEngine = RenderEngine(&viewport);

    // Main ship
    Ship::Ptr ship;

    // Test alien
    Alien::Ptr alien;

    // Asteroid handling and creation
    AsteroidHandler::Ptr asteroids;

    // Bullet handling
    BulletHandler bullets;

    // Particle handling
    ParticleHandler::Ptr particles;

    // Background
    Background bg = Background(&renderEngine);

    // Socket
    Socket::Ptr socket;

    // Messagehandler
    MessageHandler::Ptr messageHandler;

    // Camper punishment
    CamperPunisher::Ptr camperPunisher;

    // Keyboard state array
    const Uint8 *key_states;

    // User ship crash state
    bool crash = false;

    // Configuration variables
    bool fullscreen;
    bool fancy_graphics;
    bool vsync;
    int h_requested;
    int w_requested;

  private:
    int port_;
    int host_port_;
    unsigned int space_hold_ = 0;
    char *host_address_;
    bool multiplayer_;
    MultiplayerRole multiplayerRole_;

    // Text renderers
    TextEngine::Ptr magazineInfo_;
    TextEngine::Ptr livesInfo_;
    TextEngine::Ptr timeInfo_;
    TextEngine::Ptr gameOverText_;
    TextEngine::Ptr scoreInfo_;

    int ship_x_pos = 300;
    int ship_y_pos = 300;
    int ship_orientation = -90;
    int ship_w = 5;
    int ship_h = 8;

    ///
    /// \brief Advances the game in singleplayer mode
    ///
    void advanceSingleplayer_();

    ///
    /// \brief Advances the game in multiplayer mode
    ///
    void advanceMultiplayer_();

    ///
    /// \brief Called on each tick if the game has ended
    ///
    void gameOver_();

    ///
    /// \brief Called on each tick if the game is active
    ///
    void gameOn_();

    ///
    /// \brief Reads input state array and sends actions accordingly
    ///
    void handleInput_();

    ///
    /// \brief Initializes text instances
    ///
    void initText_();

    ///
    /// \brief Initializes all game objects
    ///
    void initializeGameObjects_();

    ///
    /// \brief Initializes SDL video context
    ///
    void initVideo_();

    ///
    /// \brief Updates text elements on the UI
    ///
    void updateTextContent_();

    /// \brief run all collision detections
    static void runCollisions_();

    void checkInView_();
};

#endif // GAME_H
