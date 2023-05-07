#include "background.h"
#include "rng.h"
#include "../game.h"

Background::~Background() {
    delete stars_s_;
    delete stars_m_;
    delete stars_l_;
}

Background::Background(RenderEngine *renderEngine) {
    stars_s_ =
        new Polygon(renderEngine,
                        createBackground_(STAR_S, stars_s_density,
                                          GAME_AREA_WIDTH, GAME_AREA_HEIGHT),
                        0, 0);
    stars_m_ =
        new Polygon(renderEngine,
                        createBackground_(STAR_M, stars_m_density,
                                          GAME_AREA_WIDTH, GAME_AREA_HEIGHT),
                        0, 0);
    stars_l_ =
        new Polygon(renderEngine,
                        createBackground_(STAR_L, stars_l_density,
                                          GAME_AREA_WIDTH, GAME_AREA_HEIGHT),
                        0, 0);

    stars_s_->setColor(SDL_Color{130, 100, 100, 180});
    stars_m_->setColor(SDL_Color{150, 150, 150, 200});
    stars_l_->setColor(SDL_Color{200, 200, 200, 240});

    stars_s_->setRenderType(POINT);
    stars_m_->setRenderType(POINT);
    stars_l_->setRenderType(POINT);
}

std::vector<SDL_Point>
Background::createBackground_(star_types type, double density, int w, int h) {
    std::vector<SDL_Point> background;
    int n = static_cast<int>(w * h * density);

    for (int i = 0; i < n; i++) {
        std::vector<SDL_Point> star =
                getStarShape_(type, random_int_in_range<int>(0, w),
                              random_int_in_range<int>(0, h, NORMAL));
        background.insert(background.end(), star.begin(), star.end());
    }

    return background;
}

std::vector<SDL_Point>
Background::getStarShape_(star_types type, int x, int y) {
    std::vector<SDL_Point> shape;

    switch (type) {
    case STAR_S:
        shape.push_back(SDL_Point{x, y});
        break;

    case STAR_M:
        shape.push_back(SDL_Point{x, y});
        shape.push_back(SDL_Point{x + 1, y});
        shape.push_back(SDL_Point{x - 1, y});
        shape.push_back(SDL_Point{x, y + 1});
        shape.push_back(SDL_Point{x, y - 1});
        break;

    case STAR_L:    // Not so tidy but works for static content
        shape.push_back(SDL_Point{x, y});
        shape.push_back(SDL_Point{x + 1, y});
        shape.push_back(SDL_Point{x + 2, y});
        shape.push_back(SDL_Point{x - 1, y});
        shape.push_back(SDL_Point{x - 2, y});

        shape.push_back(SDL_Point{x - 1, y - 1});
        shape.push_back(SDL_Point{x, y - 1});
        shape.push_back(SDL_Point{x + 1, y - 1});

        shape.push_back(SDL_Point{x - 1, y + 1});
        shape.push_back(SDL_Point{x, y + 1});
        shape.push_back(SDL_Point{x + 1, y + 1});

        shape.push_back(SDL_Point{x, y - 2});
        shape.push_back(SDL_Point{x, y + 2});
        break;
    }

    return shape;
}
