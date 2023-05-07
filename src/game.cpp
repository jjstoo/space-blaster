#include "game.h"
#include "config/INIReader.h"
#include "game/collisionutils.h"
#include <memory>
#include <utility>

#if BLASTER_MULTITHREAD
#include <thread>
#endif

// Statics
GameState Game::gameState;
SDL_Window *Game::WINDOW = nullptr;
SDL_Renderer *Game::RENDERER = nullptr;
Viewport *Game::VIEWPORT = nullptr;

Game::Game(bool multiplayer, int port, int host_port, char *host_address) {
    multiplayer_ = multiplayer;
    port_ = port;
    host_port_ = host_port;
    host_address_ = host_address;
    Game::VIEWPORT = &viewport;
    initializeGameObjects_();

    if (multiplayer_ && host_address_) {
        multiplayerRole_ = SLAVE;
        LOG("Started game in multiplayer mode, playing as slave");
    } else if (multiplayer && !host_address_) {
        multiplayerRole_ = MASTER;
        LOG("Started game in multiplayer mode, playing as host");
    }
}

Game::~Game() {
    SDL_DestroyWindow(Game::WINDOW);
    SDL_DestroyRenderer(Game::RENDERER);
    SDL_Quit();
}

void Game::init() {
    // Initialize video context
    initVideo_();

    // Initialize keyboard handler
    key_states = SDL_GetKeyboardState(nullptr);
    // Hide cursor
    SDL_ShowCursor(SDL_DISABLE);

    // Initialize text content
    initText_();
    gameState = ON;
}

void Game::initializeGameObjects_() {
    if (!multiplayer_) {
        particles.reset(new ParticleHandler(&physicsEngine, &renderEngine));
        asteroids.reset(new AsteroidHandler(&physicsEngine, &renderEngine,
                                            particles.get()));
        ship.reset(new Ship(&physicsEngine, &renderEngine,
                            particles.get(),
                            ship_x_pos, ship_y_pos,
                            ship_orientation,
                            ship_h, ship_w));
        viewport.track_object(ship.get());
        camperPunisher.reset(new CamperPunisher(ship, asteroids));

        if (TEST_ALIEN)
            alien.reset(new Alien(&physicsEngine, &renderEngine,
                                  ship.get(),
                                  ship_x_pos, ship_y_pos,
                                  ship_orientation,
                                  ship_h, ship_w));
    } else {
        messageHandler.reset(new MessageHandler(
            port_, host_port_, host_address_, multiplayerRole_));
        asteroids.reset(new AsteroidHandler(&physicsEngine, &renderEngine,
                                            particles.get()));
        ship.reset(new Ship(&physicsEngine, &renderEngine,
                            particles.get(),
                            ship_x_pos, ship_y_pos,
                            ship_orientation,
                            ship_h, ship_w));
        viewport.track_object(ship.get());
    }
}

void Game::initText_() {
    // Game over text, not displayed but initialized for later use
    gameOverText_.reset(new TextEngine(&renderEngine));
    gameOverText_->setFontSize(50);
    gameOverText_->setPosition(SCREEN_RES_W / 3 - 200, SCREEN_RES_H / 3);
    gameOverText_->setColor(200, 0, 0, 0);
    gameOverText_->setText("");

    // Initialize ship info text renders
    magazineInfo_.reset(
        new TextEngine(&renderEngine, 0, 0, 24));
    livesInfo_.reset(
        new TextEngine(&renderEngine, 0, 30, 24));
    timeInfo_.reset(
        new TextEngine(&renderEngine, 0, 60, 24));
    livesInfo_->setColor(255, 255, 255, 255);
    magazineInfo_->setColor(255, 255, 255, 255);
    magazineInfo_->
        setText("Bullets: " + std::to_string(
                                  ship->getActiveWeapon()->getShotsLeft()));
    livesInfo_->setText("HP: " + std::to_string(ship->getLives()));
    timeInfo_->setText("Time: " + std::to_string(ship->getTimeAlive()));

    // Score text
    scoreInfo_.reset(new TextEngine(&renderEngine));
    scoreInfo_->setFontSize(24);
    scoreInfo_->setPosition(0, 90);
    scoreInfo_->setColor(0,0,255,255);
    scoreInfo_->setText("Score: ");
}

void Game::initVideo_() {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        ERROR("SDL could not initialize! SDL Error: %s", SDL_GetError());

    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, fancy_graphics ? "2" : "0"))
        LOG("Warning: Linear texture filtering not enabled!");

    // Enable or disable vsync
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync ? "1" : "0");
    LOG("Vsync: %s", vsync ? "on" : "off");

    // Create window
    Game::WINDOW =
        SDL_CreateWindow(SPACE_BLASTER_VERSION,
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         w_requested, h_requested,
                         (SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI));
    if (Game::WINDOW == nullptr)
        ERROR("Window could not be created! SDL Error: %s", SDL_GetError());

    // Set fullscreen if desired
    if (fullscreen)
        SDL_SetWindowFullscreen(Game::WINDOW, SDL_WINDOW_FULLSCREEN_DESKTOP);

    // Create renderer
    Game::RENDERER =
        SDL_CreateRenderer(Game::WINDOW, -1, SDL_RENDERER_ACCELERATED);
    if (Game::RENDERER == nullptr)
        ERROR("Renderer could not be created! SDL Error: %s", SDL_GetError());

    // Scale graphics accordingly
    int w;
    int h;
    SDL_GetWindowSize(Game::WINDOW, &w, &h);
    double w_scale = (double)w / (double)SCREEN_RES_W;
    double h_scale = (double)h / (double)SCREEN_RES_H;
    LOG("Render scaling factors: %f %f", w_scale, h_scale);
    if (SDL_RenderSetScale(Game::RENDERER, w_scale, h_scale) < 0)
        ERROR("SDL render scale failed: %s", SDL_GetError());

    // Initialize renderer color
    SDL_SetRenderDrawColor(Game::RENDERER, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_SetRenderDrawBlendMode(Game::RENDERER, SDL_BLENDMODE_NONE);
}

void Game::configure(std::string conf) {
    INIReader config(std::move(conf));
    if (config.ParseError() != 0)
        ERROR("Error reading configuration");

    fullscreen = config.GetBoolean("game", "fullscreen", false);
    w_requested = static_cast<int>(config.GetInteger("game", "width", 1280));
    h_requested = static_cast<int>(config.GetInteger("game", "height", 720));
    fancy_graphics = config.GetBoolean("game", "fancyGraphics", false);
    vsync = config.GetBoolean("game", "vsync", true);
}

void Game::advance() {
    if (!ship->alive)
        gameState = ENDED;

    if (!multiplayer_) {
        advanceSingleplayer_();
    } else {
        advanceSingleplayer_();
        MessageBuffer shipMessage =
            messageHandler->buildShipMessage(ship.get());
        // socket->send(shipMessage.GetBufferPointer(), shipMessage.GetSize());
    }
}

void Game::advanceSingleplayer_() {
    if (gameState == ON) {
        gameOn_();
    } else if (gameState == ENDED) {
        gameOver_();
    }

    checkInView_();
    runCollisions_();

    if (ship->isDamageTaken()) {
        SDL_SetRenderDrawColor(Game::RENDERER, 0x50, 0x00, 0x10, 0x00);
        crash = false;
    } else {
        SDL_SetRenderDrawColor(Game::RENDERER, 0x00, 0x00, 0x10, 0x00);
    }
    SDL_RenderClear(Game::RENDERER);

    // Run update tasks
    particles->update();
    asteroids->update();
    camperPunisher->update();
    bullets.update();
    renderEngine.render();
    ship->update();


    // Update viewport
    viewport.update();

    // Render everything onto screen
    // SDL_RenderSetViewport(Game::RENDERER, viewport.get());
    SDL_RenderPresent(Game::RENDERER);

    // Advance a step in the physics engine.
    // This executes all forces/actions stored to the physics objects, and
    // calculates the new physics of the object for the next frame.
    physicsEngine.step();

    // Update text elements
    updateTextContent_();

}

void Game::gameOver_() {
    if (key_states[SDL_SCANCODE_SPACE]) {
        if (space_hold_ == 0) {
            space_hold_ = SDL_GetTicks();
        } else if (((SDL_GetTicks() - space_hold_) > 800)) {
            // Space pressed for 800ms, restart
            gameOverText_->setText("");
            livesInfo_->setColor(255, 255, 255, 255);
            space_hold_ = 0;
            asteroids->resetAsteroids();
            ship->reset(300, 300, -90);
            gameState = ON;
            return;
        }
    } else {
        space_hold_ = 0;
    }
    unsigned int t = ship->getTimeAlive();
    gameOverText_->setText("You survived " + std::to_string(t) +
                           " seconds. Hold space to restart");
}

void Game::gameOn_() {
    // Handle user input
    handleInput_();

    // Update asteroid spawn time based on craft time alive
    unsigned int time = ship->getTimeAlive();
    if (time < 100) {
        asteroids->setSpawnInterval(AsteroidHandler::DEFAULT_SPAWN_INTERVAL -
                                time * 10);
        asteroids->setSpawnSpeed(3 + (time / 10));
    }
}

void Game::updateTextContent_() {
    timeInfo_->setText("Time: " + std::to_string(ship->getTimeAlive()));
    if (!ship->getActiveWeapon()->isReloading()) {
        magazineInfo_->setColor(255, 255, 255, 255);
        magazineInfo_->setText(ship->getActiveWeapon()->getName() + ": " +
                               std::to_string(ship->getActiveWeapon()->getShotsLeft()));
    } else {
        magazineInfo_->setColor(255, 0, 0, 255);
        magazineInfo_->setText(ship->getActiveWeapon()->getName() + " reloading");
    }

    if (ship->getLives() == 1) {
        livesInfo_->setColor(255, 0, 0, 255);
    }

    livesInfo_->setText("HP: " + std::to_string(ship->getLives()));
    scoreInfo_->setText("Score: " + std::to_string(ship->getScore()));
}

void Game::handleInput_() {
    // Handle keyboard events
    // These events are handled without debouncing and at a VERY high speed,
    // it is not wise to include menu/exit/other non-gaming keys here due to
    // multiple presses being detected instantly
    if ((key_states[SDL_SCANCODE_LEFT] && !key_states[SDL_SCANCODE_RIGHT]) ||
        (key_states[SDL_SCANCODE_A] && !key_states[SDL_SCANCODE_D])) {
        ship->rotate(Ship::Rotation::Anticlockwise);
    }

    if ((key_states[SDL_SCANCODE_RIGHT] && !key_states[SDL_SCANCODE_LEFT]) ||
        (key_states[SDL_SCANCODE_D] && !key_states[SDL_SCANCODE_A])) {
        ship->rotate(Ship::Rotation::Clockwise);
    }

    if ((key_states[SDL_SCANCODE_UP]) || (key_states[SDL_SCANCODE_W])) {
        ship->accelerate();
    }

    if (key_states[SDL_SCANCODE_SPACE]) {
        ship->shoot(&bullets);
    }

    if (key_states[SDL_SCANCODE_R]) {
        ship->reload();
    }

    // Weapons switching
    if (key_states[SDL_SCANCODE_1]) {
        ship->switchWeapon(0);
    }

    if (key_states[SDL_SCANCODE_2]) {
        ship->switchWeapon(1);
    }

    if (key_states[SDL_SCANCODE_3]) {
        ship->switchWeapon(2);
    }

    if (key_states[SDL_SCANCODE_4]) {
        ship->switchWeapon(3);
    }
}


void Game::checkInView_() {
    auto entities = Entity::getEntities();
    for (auto & e : entities) {
        if (auto b = e->getBody()) {
            // TODO
        }

    }
}

void Game::runCollisions_() {
    auto entities = Entity::getEntities();
    auto it = entities.begin();
    auto end = entities.end();

    while (it != end) {
        auto e1 = *it;

        // Skip non-collidables
        if (!e1->isCollidable()) {
            ++it;
            continue;
        }

        auto it2 = it;
        it2++;
        while (it2 != end) {
            auto e2 = *it2;

            // Skip non-collidables
            if (!e2->isCollidable()) {
                ++it2;
                continue;
            }

            // Directly skip entities far away
            if (!e1->getBody()->isCloseToQ(e2->getBody())) {
                ++it2;
                continue;
            }

            // Check if these types can collide with each other
            if (!e1->doesCollideWith(e2->getType()) ||
                !e2->doesCollideWith(e1->getType())) {
                ++it2;
                continue;
            }

            if (e1->getBody()->intersects(e2->getBody())) {
                e1->collisionWith(e2);
                e2->collisionWith(e1);
            }
            ++it2;
        }
        ++it;
    }
}
