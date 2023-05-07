#include "viewport.h"

Viewport::Viewport(int buffer) {
    buffer_ = buffer;
    offset_x_max_ = GAME_AREA_WIDTH - SCREEN_RES_W;
    offset_y_max_ = GAME_AREA_HEIGHT - SCREEN_RES_H;
    vp_ = new SDL_Rect;
}

Viewport::~Viewport() { delete vp_; }

void Viewport::track_object(PhysicsObject *obj) { target_ = obj; }

void Viewport::update() {
    int x = static_cast<int>(target_->getPosX());
    int y = static_cast<int>(target_->getPosY());

    int upper_boundary;
    int lower_boundary;

    // x -direction
    upper_boundary = offset_x_ + SCREEN_RES_W - buffer_;
    lower_boundary = offset_x_ + buffer_;

    if (x > upper_boundary) {
        offset_x_ += x - upper_boundary;
    } else if (x < lower_boundary) {
        offset_x_ += x - lower_boundary;
    }

    if (offset_x_ < 0)
        offset_x_ = 0;
    else if (offset_x_ > offset_x_max_)
        offset_x_ = offset_x_max_;

    // y -direction
    upper_boundary = offset_y_ + SCREEN_RES_H - buffer_;
    lower_boundary = offset_y_ + buffer_;

    if (y > upper_boundary) {
        offset_y_ += y - upper_boundary;
    } else if (y < lower_boundary) {
        offset_y_ += y - lower_boundary;
    }

    if (offset_y_ < 0)
        offset_y_ = 0;
    else if (offset_y_ > offset_y_max_)
        offset_y_ = offset_y_max_;
}

SDL_Rect *Viewport::get() {
    vp_->x = -offset_x_;
    vp_->y = -offset_y_;
    vp_->w = SCREEN_RES_W + offset_x_;
    vp_->h = SCREEN_RES_H + offset_y_;

    return vp_;
}

bool Viewport::isPointInView(int x, int y) {
    // TODO
}


