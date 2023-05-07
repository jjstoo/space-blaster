#include "blaster.h"
#include "SDL2/SDL.h"
#include "game.h"

#ifdef _WIN32
#include <time.h>
#endif

double g_timescale = 1.0;

void parse_arguments(int argc, char *argv[], bool *multiplayer, int *port,
                     int *host_port, char **host_addr) {
    for (int i = 1; i < argc; ++i) {

        // Multiplayer mode
        if (strcmp(argv[i], "-m") == 0) {
            *multiplayer = true;
        }

        // Port selection
        else if (strcmp(argv[i], "-p") == 0) {
            if (argc > (i + 1)) {
                int p = atoi(argv[i + 1]);
                if (p == 0)
                    ERROR("Invalid UDP port %d selected", p);
                *port = p;
                ++i;
            }
        }

        // Host port selection
        else if (strcmp(argv[i], "-hp") == 0) {
            if (argc > (i + 1)) {
                int p = atoi(argv[i + 1]);
                if (p == 0)
                    ERROR("Invalid host UDP port %d selected", p);
                *host_port = p;
                ++i;
            }
        }

        // Host address
        else if (strcmp(argv[i], "-ha") == 0) {
            if (argc > (i + 1)) {
                *host_addr = argv[i + 1];
                ++i;
            }
        }

        // Help print
        else if (strcmp(argv[i], "-h") == 0) {
            printf("%s", help_str);
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char *argv[]) {

    LOG("Starting Space-Blaster %s", SPACE_BLASTER_VERSION);

    // Default values for arguments
    bool multiplayer = false;
    int port = DEFAULT_PORT;
    int host_port = DEFAULT_PORT;
    char *host_addr = nullptr;

    parse_arguments(argc, argv, &multiplayer, &port, &host_port, &host_addr);

    // Initialize truetype fonts
    if (TTF_Init() == -1)
        ERROR("TTF_Init: %s\n", TTF_GetError());

    // Game instance
    Game::Ptr game =
        Game::Ptr(new Game(multiplayer, port, host_port, host_addr));
    game->configure("options.ini");
    game->init();

    // Exit flag
    bool quit = false;

    // User input event handling
    SDL_Event e;

    // Seed for the RNG
    srand(static_cast<unsigned int>(time(nullptr)));

#if SHOW_FPS
    TextEngine::Ptr perfText =
        TextEngine::Ptr(new TextEngine(&game->renderEngine));
    perfText->setFontSize(12);
    perfText->setPosition(10, 1000);
    perfText->setColor(255, 255, 255, 255);
#endif

    // Record last frame timestamp
    Uint32 last_step = SDL_GetTicks();
    Uint32 delta = 0;

    while (!quit) {

        // Record current time
        Uint32 frame_start = SDL_GetTicks();

        // Record delta time for the frame and adjust timescale accordingly
        delta = frame_start - last_step;
        g_timescale =
            static_cast<double>(delta) / static_cast<double>(TICKS_PER_FRAME);

        // Run input updates
        while (SDL_PollEvent(&e) != 0) {
            // Exit button or Escape pressed
            if ((e.type == SDL_QUIT) || ((e.type == SDL_KEYDOWN) &&
                                         (e.key.keysym.sym == SDLK_ESCAPE))) {
                quit = true;
            }
        }

        // Run game for one tick forward
        game->advance();
        last_step = frame_start;

#if CAP_FPS
        // Cap at max fps
        Uint32 frame_end = SDL_GetTicks();
        if ((frame_end - frame_start) < TICKS_PER_MAX_FPS) {
            SDL_Delay(TICKS_PER_MAX_FPS - (frame_end - frame_start));
        }
#endif

#if SHOW_FPS
        // LOG("Delta %d Timescale %f", delta, timescale);
        perfText->setText("FPS: " + std::to_string((1 / g_timescale) * 60));
#endif
    }

    return EXIT_SUCCESS;
}
