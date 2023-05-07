#include "graphics.h"
#include "../game.h"
#include <iostream>
#include <utility>

Polygon::~Polygon() {
    delete[] outline;
    delete[] render_buffer;
    delete[] fill_render_buffer;
}

Polygon::Polygon(): RenderObject()  {}

Polygon::Polygon(RenderEngine *renderEngine,
                 std::vector<SDL_Point> outline, int x, int y) :
    RenderObject() {
    init(renderEngine, std::move(outline), x, y);
}

Polygon::Polygon(RenderEngine *renderEngine, SDL_Point *outline,
                         int points, int x, int y) :
    RenderObject(renderEngine) {
    x_ = x;
    y_ = y;
    this->x = x;
    this->y = y;
    this->outline = outline;
    this->points = points;

    double x_temp, y_temp, r_temp;
    for (unsigned long i = 0; i < static_cast<unsigned long>(points); i++) {
        x_temp = outline[i].x;
        y_temp = outline[i].y;
        this->coordinates.push_back(Point{x_temp, y_temp});
        r_temp = CoordinateUtils::distance(outline[i], SDL_Point{x, y});
        if (r_temp > max_r_)
            max_r_ = r_temp;
    }

    render_buffer = new SDL_Point[points];
}

void Polygon::init(RenderEngine *renderEngine,
                   std::vector<SDL_Point> initial_outline,
                   int x, int y) {
    RenderObject::init(renderEngine);
    renderEngine_ = renderEngine;

    x_ = x;
    y_ = y;
    this->x = x;
    this->y = y;
    points = static_cast<int>(initial_outline.size());
    this->outline = new SDL_Point[initial_outline.size()];

    double x_temp, y_temp, r_temp;
    for (unsigned long i = 0; i < static_cast<unsigned long>(points); i++) {
        this->outline[i] = initial_outline[i];
        x_temp = initial_outline[i].x;
        y_temp = initial_outline[i].y;
        this->coordinates.push_back(Point{x_temp, y_temp});
        r_temp = CoordinateUtils::distance(initial_outline[i], SDL_Point{x, y});
        if (r_temp > max_r_)
            max_r_ = r_temp;
    }

    render_buffer = new SDL_Point[points];
}

void Polygon::updateOutline_() {
    int x_temp, y_temp;
    for (unsigned long i = 0; i < static_cast<unsigned long>(points); i++) {
        Point coord = coordinates[i];
        x_temp = static_cast<int>(coord.x);
        y_temp = static_cast<int>(coord.y);
        outline[i] = SDL_Point{x_temp, y_temp};
    }
}

void Polygon::updateCenterPoint_() {
    x = static_cast<int>(x_);
    y = static_cast<int>(y_);
}

void Polygon::updateExtremes_() {
    max_x_ = static_cast<int>(max_x_d_);
    min_x_ = static_cast<int>(min_x_d_);
    max_y_ = static_cast<int>(max_y_d_);
    min_y_ = static_cast<int>(min_y_d_);

}

void Polygon::move(double amount_x, double amount_y) {
    int x_old = x;
    int y_old = y;
    x_ += amount_x;
    y_ += amount_y;

    max_x_d_ += amount_x;
    min_x_d_ += amount_x;
    max_y_d_ += amount_y;
    min_y_d_ += amount_y;

    for (unsigned long i = 0; i < static_cast<unsigned long>(points); i++) {
        coordinates[i].x += amount_x;
        coordinates[i].y += amount_y;
    }

    updateCenterPoint_();
    updateOutline_();
    updateExtremes_();
    if (renderType_ == FILL)
        moveFill_(x - x_old, y - y_old);
}

void Polygon::moveAbsolute(double dest_x, double dest_y) {
    move(dest_x - x_, dest_y - y_);
}

bool Polygon::intersects(Polygon *p) {
    if (!isCloseTo(p))
        return false;

    SDL_Point *a,*b,*c,*d;

    for (int i = 0; i < points - 1; i++) {
        a = &outline[i];
        b = &outline[i + 1];
        for (int j = 0; j < p->points - 1; j++) {
            c = &p->outline[j];
            d = &p->outline[j + 1];
            if (CoordinateUtils::line_intersection(a, b, c, d))
                return true;
        }
    }

    // The polygons could be inside each other, check the center point
    SDL_Point point{p->x, p->y};
    if (contains(&point))
        return true;

    return false;
}

void Polygon::rotate(double angle_rad, int origin_x, int origin_y) {
    for (unsigned long i = 0; i < static_cast<unsigned long>(points); i++) {
        coordinates[i] = CoordinateUtils::rotate_point(
                    origin_x, origin_y, angle_rad, coordinates[i]);
    }

    updateOutline_();
    maxValUpdated_ = false;
}

bool Polygon::outOfBounds(int buffer) {
    for (int i = 0; i < points; i++) {
        SDL_Point p = outline[i];
        if (!CoordinateUtils::check_out_of_bounds(p.x, p.y, buffer)) {
            return false;
        }
    }

    return true;
}

bool Polygon::isCloseToQ(Polygon *p) {
    static int delta = 5;
    double d = CoordinateUtils::distance(SDL_Point{x, y},
                                         SDL_Point{p->x, p->y});

    if ((d + delta) < (max_r_ + p->getMaxRadius()))
        return true;
    return false;
}

bool Polygon::isCloseTo(Polygon *p) {
    static int delta = 5;
    bool xRange = (getMaxX() + delta > p->getMinX()) &&
                  (getMinX() < p->getMaxX() + delta);
    bool yRange = (getMaxY() + delta > p->getMinY()) &&
                  (getMinY() < p->getMaxY() + delta);

    return (xRange && yRange);
}

bool Polygon::isCloseTo(SDL_Point *p) {
    bool xRange = (getMaxX() > p->x) && (getMinX() < p->x);
    bool yRange = (getMaxY() > p->y) && (getMinY() < p->y);

    return (xRange && yRange);
}

void Polygon::searchExtremes_() {
    max_x_d_ = getExtremePoint_(X, MAX);
    min_x_d_ = getExtremePoint_(X, MIN);
    max_y_d_ = getExtremePoint_(Y, MAX);
    min_y_d_ = getExtremePoint_(Y, MIN);
    updateExtremes_();
    maxValUpdated_ = true;
}

int Polygon::getExtremePoint_(Dimension d, Extremity e) const {
    int extremity;

    for (int i = 0; i < points; i++) {
        int candidate;
        SDL_Point *p = &outline[i];

        if (d == X) {
            candidate = p->x;
        } else if (d == Y) {
            candidate = p->y;
        }

        if (i == 0)
            extremity = candidate;
        else {
            if (e == MAX) {
                if (candidate > extremity)
                    extremity = candidate;
            } else if (e == MIN) {
                if (candidate < extremity)
                    extremity = candidate;
            }
        }
    }

    return extremity;
}

void Polygon::setColor(SDL_Color color) { color_ = color; }

void Polygon::setRenderType(RenderType renderType) {
    renderType_ = renderType;
    if (renderType_ == FILL) {

        if (!checkClosedOutline_(outline))
            throw std::invalid_argument("Outline is not enclosed");

        fill = new SDL_Point[GAME_AREA_HEIGHT * 2];
        fill_render_buffer = new SDL_Point[GAME_AREA_HEIGHT * 2];
        calculateFill_();
    }
}

void Polygon::render(int offset_x, int offset_y) {
    CoordinateUtils::translate(render_buffer, outline,
                             points, offset_x, offset_y);
    SDL_SetRenderDrawColor(Game::RENDERER,
                           color_.r,
                           color_.g,
                           color_.b,
                           color_.a);

    if (renderType_ == LINE || renderType_ == FILL)
        SDL_RenderDrawLines(Game::RENDERER, render_buffer, points);
    else if (renderType_ == POINT)
        SDL_RenderDrawPoints(Game::RENDERER, render_buffer, points);

    if (renderType_ == FILL) {
        CoordinateUtils::translate(fill_render_buffer, fill,
                                   n_fill_points, offset_x, offset_y);
        SDL_RenderDrawLines(Game::RENDERER, fill_render_buffer, n_fill_points);
    }
}

SDL_Color Polygon::getColor() { return color_; }

int Polygon::getMaxX() {
    if (!maxValUpdated_)
        searchExtremes_();
    return max_x_;
}
int Polygon::getMinX() {
    if (!maxValUpdated_)
        searchExtremes_();
    return min_x_;
}
int Polygon::getMaxY() {
    if (!maxValUpdated_)
        searchExtremes_();
    return max_y_;
}
int Polygon::getMinY() {
    if (!maxValUpdated_)
        searchExtremes_();
    return min_y_;
}

double Polygon::getMaxRadius() const {
    return max_r_;
}

void Polygon::calculateFill_() {
    if (!outline)
        return;

    // Get the min and max y points to determine the horizontal scanline limits
    int min_y = getMinY();
    int max_y = getMaxY();

    n_fill_points = 0;
    for (int y_idx = min_y; y_idx < max_y; y_idx++) {
        SDL_Point a1 = {static_cast<int>(-GAME_AREA_WIDTH * 0.5), y_idx};
        SDL_Point a2 = {static_cast<int>(GAME_AREA_WIDTH * 1.5), y_idx};
        SDL_Point intersection;

        // Find scanline start and end points
        for (int i = 0; i < points - 1; i++) {
            int n_intersects = 0;
            if (CoordinateUtils::line_intersection(
                        &a1, &a2, &outline[i], &outline[i + 1])) {
                if (CoordinateUtils::line_intersection_point(
                        &a1, &a2, &outline[i], &outline[i + 1],
                        &intersection)) {
                    fill[n_fill_points] = intersection;
                    if (++n_fill_points > GAME_AREA_HEIGHT * 2)
                        break;
                    if (++n_intersects == 2)
                        break;
                }
            }
        }

        // Safety measures to avoid segmentation faults
        if (n_fill_points > GAME_AREA_HEIGHT * 2) {
            n_fill_points = GAME_AREA_HEIGHT * 2;
            break;
        }
    }
}

void Polygon::moveFill_(int x_amount, int y_amount) {
    for (int i = 0; i < n_fill_points; ++i) {
        fill[i].x += x_amount;
        fill[i].y += y_amount;
    }
}

void Polygon::rotate(double angle_rad) {
    Polygon::rotate(angle_rad, x, y);
    if (renderType_ == FILL)
        calculateFill_();
}

bool Polygon::contains(SDL_Point *p) const {
    // Efficient winding algorithm
    // http://geomalgorithms.com/a03-_inclusion.html
    int wn = 0;
    SDL_Point *V = outline;
    for (int i = 0; i < points - 1; i++) {
        if (V[i].y <= p->y) {
            if (V[i + 1].y > p->y)
                if (CoordinateUtils::check_side(p, &V[i], &V[i + 1]) > 0)
                    ++wn;
        } else {
            if (V[i + 1].y <= p->y)
                if (CoordinateUtils::check_side(p, &V[i], &V[i + 1]) < 0)
                    --wn;
        }
    }

    return (wn != 0);
}

ScreenPosition Polygon::getScreenPosition() const {
    if (x < (GAME_AREA_WIDTH / 2)) {
        if (y < (GAME_AREA_HEIGHT / 2))
            return ScreenPosition::NW;
        else
            return ScreenPosition::SW;
    } else {
        if (y < (GAME_AREA_HEIGHT / 2))
            return ScreenPosition::NE;
        else
            return ScreenPosition::SE;
    }
}

