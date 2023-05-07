#ifndef BLASTER_H
#define BLASTER_H

#include "SDL2/SDL_messagebox.h"
#include <stdexcept>

// ------ Common types, constant members, etc ------ //
extern double g_timescale;
enum GameState {
    WAIT_FOR_START,
    ON,
    ENDED,
};

enum MultiplayerRole { MASTER, SLAVE };

static const char *const help_str = R"HELP(
SPACE BLASTER  -   A classic space shoot 'em up

Usage: blaster OPTION...
Optional arguments:
    -m,             Run in multiplayer
    -p <port>,      Use given UDP port
    -ha <address>,  Set host address.
                    Pass only if you wish to play multiplayer as a slave.
    -hp <port>,     Set host UDP port.
                    Pass only if you wish to play multiplayer as a slave.
    -h,             Print this help
)HELP";

static const int GAME_AREA_HEIGHT = 4230;
static const int GAME_AREA_WIDTH = 7680;

static const int SCREEN_RES_H = 1080;
static const int SCREEN_RES_W = 1920;

static const int GAME_LOGIC_HZ = 60;
static const int MAX_FPS = 160;
static const int TICKS_PER_FRAME = 1000 / GAME_LOGIC_HZ;
static const int TICKS_PER_MAX_FPS = 1000 / MAX_FPS;

static const double PI = 3.1415927;
static const double RAD2DEG = 57.2957795;
static const double DEG2RAD = 1.0 / RAD2DEG;

// ------ Compile-time options ------ //
#define SPACE_BLASTER_VERSION "0.0.1"

// Threading
#define BLASTER_MULTITHREAD 0

// Networking
#define DEFAULT_PORT 2000
#define PACKET_SIZE 1024

// Enable visual physics debugging
#define PHYSICS_VISUAL_DEBUG 0

// Enable performance logging
#define SHOW_FPS 1

// Cap at MAX_FPS
#define CAP_FPS 1

// RenderEngine debug messages
#define DEBUG_RENDERING 0

// Enable debug print for collision details
#define DEBUG_PHYSICS_COLLISIONS 0

// Disables asteroid spawning, use one at a time
#define DEBUG_NO_ASTEROIDS 0
#define DEBUG_ONE_ASTEROID 0
#define DEBUG_TWO_ASTEROID_COLLISION 0

// Draws bounding lines over asteroid extreme coordinates
#define DEBUG_ASTEROID_BORDERS 0

// Test AI alien
#define TEST_ALIEN 0

#define N_RENDER_LAYERS 8
#define TOP_RENDER_LAYER_IDX (N_RENDER_LAYERS - 1)
#define BOTTOM_RENDER_LAYER_IDX 0

// ------ Macros ------ //
#define FILENAME                                                               \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1   \
                                      : __FILE__)

#define LOG(fmt_, ...)                                                         \
    fprintf(stderr, ("%s::%d - " fmt_ "\n"), FILENAME, __LINE__, ##__VA_ARGS__)

#define ERROR(fmt_, ...)                                                       \
    {                                                                          \
        char *buffer =                                                         \
            (char *)malloc(snprintf(NULL, 0, ("%s::%d - " fmt_ "\n"),          \
                                    FILENAME, __LINE__, ##__VA_ARGS__) +       \
                           1);                                                 \
                                                                               \
        sprintf(buffer, ("%s::%d - " fmt_ "\n"), FILENAME, __LINE__,           \
                ##__VA_ARGS__);                                                \
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", buffer, NULL); \
                                                                               \
        throw std::runtime_error("Fatal runtime error");                       \
    }                                                                          \
    void _enforce_semicolon_()

#endif // BLASTER_H
