#include "physicsobject.h"
#include "../game.h"
#include "../game/graphics.h"
#include <limits>
#include <vector>

PhysicsObject::PhysicsObject(PhysicsEngine *engine, double radius,
                             double x_initial, double y_initial,
                             bool allow_out_of_bounds, double max_speed,
                             bool friction_decay)
    : physicsEngine_(engine),
      allow_out_of_bounds_(allow_out_of_bounds),
      friction_decay_(friction_decay),
      ticks_(SDL_GetTicks()) {
    if (max_speed < 0) {
        max_speed_ = std::numeric_limits<double>::max();
    } else {
        max_speed_ = max_speed;
    }

    setRadius(radius);
    resetPhysicsState(x_initial, y_initial);
    if (physicsEngine_)
        physicsEngine_->addObject(this);
}

PhysicsObject::~PhysicsObject() {
    if (physicsEngine_)
        physicsEngine_->removeObject(this);
}

void PhysicsObject::calculatePhysics(double friction_decay, bool visual_debug) {
    // THIS FUNCTION SHOULD BE ONLY CALLED BY THE PHYSICS ENGINE

    // Calculate the new position
    Uint32 ticks_current = SDL_GetTicks();
    double time_delta = (double)(ticks_current - ticks_); // ticks
    x_ += vx_ * time_delta;
    y_ += vy_ * time_delta;

    // Calculate current acceleration based on the currently applied forces
    ax_ = (Fx_ + Fx_const_) / m_;
    ay_ = (Fy_ + Fy_const_) / m_;

    // Calculate new velocity vector, also make sure the maximum speed is not
    // exceeded
    vx_ += ax_ * time_delta;
    vy_ += ay_ * time_delta;
    double v_total = std::sqrt(std::pow(vx_, 2) + std::pow(vy_, 2));
    if (v_total > max_speed_) {
        // Scale vector to the max speed
        vx_ = vx_ / v_total * max_speed_;
        vy_ = vy_ / v_total * max_speed_;
    }

    // Speed decay due to fluid friction, 0 represents a vacuum
    if (friction_decay_) {
        vx_ *= (1.0f - friction_decay * g_timescale);
        vy_ *= (1.0f - friction_decay * g_timescale);
    }

    // If the object hits the game screen bounds, and out of bounds is not
    // allowed, reset velocity and acceleration
    checkBounds();

    // Kinetic energy
    E_kx_ = 0.5 * m_ * std::pow(vx_, 2);
    E_ky_ = 0.5 * m_ * std::pow(vy_, 2);

    if (visual_debug) {
        // LOG("mass: %.2f, x: %.2f, y: %.2f, vx: %.2f, vy: %.2f, ax:
        // %.2f, ay: %.2f, Fx: %.2f, Fy: %.2f, Time delta: %.2f",
        // m_, x_, y_, vx_, vy_, ax_, ay_,  (Fx_ + Fx_const_), (Fy_+ Fy_const_),
        // time_delta)
        renderDebugPhysics();
    }

    // Reset forces. This means that the game engine must set forces each frame
    // itself. Store also current game ticks for next physics step
    Fx_ = 0.0f;
    Fy_ = 0.0f;
    ticks_ = ticks_current;
}

void PhysicsObject::setRadius(double radius) {
    // Make sure the radius is valid, if so, calculate the mass of the object
    // and set state
    assert((radius > 0) && (radius < GAME_AREA_HEIGHT));
    r_ = radius;
    double V = PI * std::pow(r_, 2); // circle "2D volume"
    m_ = DENSITY * V;
}

void PhysicsObject::setSpeed(double angle_rad, double magnitude) {
    vx_ = std::cos(angle_rad) * magnitude;
    vy_ = std::sin(angle_rad) * magnitude;
    E_kx_ = 0.5 * m_ * std::pow(vx_, 2);
    E_ky_ = 0.5 * m_ * std::pow(vy_, 2);
}

void PhysicsObject::setSpeedXY(double vx, double vy) {
    vx_ = vx;
    vy_ = vy;
    E_kx_ = 0.5 * m_ * std::pow(vx_, 2);
    E_ky_ = 0.5 * m_ * std::pow(vy_, 2);
}

void PhysicsObject::checkBounds() {
    if (!allow_out_of_bounds_) {
        // x axis
        if (x_ < 0) {
            x_ = 0;
            vx_ = 0;
            ax_ = 0;
        } else if (x_ > GAME_AREA_WIDTH) {
            x_ = GAME_AREA_WIDTH;
            vx_ = 0;
            ax_ = 0;
        }

        // y axis
        if (y_ < 0) {
            y_ = 0;
            vy_ = 0;
            ay_ = 0;
        } else if (y_ > GAME_AREA_HEIGHT) {
            y_ = GAME_AREA_HEIGHT;
            vy_ = 0;
            ay_ = 0;
        }
    }
}

void PhysicsObject::setMaxSpeed(double max_speed) { max_speed_ = max_speed; }

void PhysicsObject::addForce(double angle_rad, double magnitude) {
    Fx_ += std::cos(angle_rad) * magnitude;
    Fy_ += std::sin(angle_rad) * magnitude;
}

void PhysicsObject::addForceToPoint(double x, double y, double magnitude) {
    double angle_rad = std::atan2((y - y_), (x - x_));
    addForce(angle_rad, magnitude);
}

void PhysicsObject::setConstantForce(double angle_rad, double magnitude) {
    Fx_const_ = std::cos(angle_rad) * magnitude;
    Fy_const_ = std::sin(angle_rad) * magnitude;
}

bool PhysicsObject::isApproaching(PhysicsObject *obj) {
    double d0 = CoordinateUtils::distance(Point {getPosX(), getPosY()},
                         Point {obj->getPosX(), obj->getPosY()});
    // Simulate next position
    Point a1 = Point {getPosX() + getVelX(),
            getPosY() + getVelY()};
    point b1 = Point {obj->getPosX() + obj->getVelX(),
            obj->getPosY() + obj->getVelY()};

    if (CoordinateUtils::distance(a1, b1) < d0)
        return true;
    return false;
}

double PhysicsObject::physicsCollisionWith(PhysicsObject *obj) {
    // TODO: Energy is almost but not fully preserved, recheck the equations pls
    // https://williamecraver.wixsite.com/elastic-equations

    // Objects sometimes tangle with each other if the visual models overlap.
    // If the models are already moving away from each other, the collision
    // is most likely a glitch due to overlap.
    if (!isApproaching(obj))
        return 0;

    double m1 = getMass();
    double vx1 = getVelX();
    double vy1 = getVelY();

    double m2 = obj->getMass();
    double vx2 = obj->getVelX();
    double vy2 = obj->getVelY();

    double px_old = m1 * vx1;
    double py_old = m1 * vy1;

    double vx1_new = (vx1 * (m1 - m2) + (2 * m2 * vx2)) / (m1 + m2);
    double vy1_new = (vy1 * (m1 - m2) + (2 * m2 * vy2)) / (m1 + m2);
    double vx2_new = (vx2 * (m2 - m1) + (2 * m1 * vx1)) / (m1 + m2);
    double vy2_new = (vy2 * (m2 - m1) + (2 * m1 * vy1)) / (m1 + m2);

    #if DEBUG_PHYSICS_COLLISIONS
    double Ek1_old = getKineticEMag();
    double Ek2_old = obj->getKineticEMag();
    double angle_old_1 = getVelAngle();
    double angle_old_2 = obj->getVelAngle();
    #endif

    setSpeedXY(vx1_new, vy1_new);
    obj->setSpeedXY(vx2_new, vy2_new);

    double px = m1 * getVelX();
    double py = m1 * getVelY();
    double I = std::sqrt(std::pow(px - px_old, 2) + std::pow(py - py_old, 2));

    #if DEBUG_PHYSICS_COLLISIONS
    double Ek1_new = getKineticEMag();
    double Ek2_new = obj->getKineticEMag();

    LOG("COLLISION:\n"
        "Impulse: %f\n"
        "A - Delta E: %f, m: %f, dir old: %f, dir new: %f\n"
        "B - Delta E: %f, m: %f, dir old: %f, dir new: %f",
        I,
        std::abs(Ek1_old - Ek1_new), m1, angle_old_1, getVelAngle(),
        std::abs(Ek2_old - Ek2_new), m2, angle_old_2, obj->getVelAngle());
    #endif

    return I;
}

void PhysicsObject::resetPhysicsState(double x, double y) {
    x_ = x;
    y_ = y;
    vx_ = 0;
    vy_ = 0;
    ax_ = 0;
    ay_ = 0;
    Fx_ = 0;
    Fy_ = 0;
    Fx_const_ = 0;
    Fy_const_ = 0;
    E_kx_ = 0;
    E_ky_ = 0;
    ticks_ = SDL_GetTicks();
}

void PhysicsObject::renderDebugPhysics() {
    // Force vector and object radius circle
    SDL_Point force_vec[2] = {
        {static_cast<int>(x_), static_cast<int>(y_)},
        {static_cast<int>(x_ + ((Fx_ + Fx_const_) * 1000)),
         static_cast<int>(y_ + ((Fy_ + Fy_const_) * 1000))}};
    SDL_Point velocity_vec[2] = {{static_cast<int>(x_), static_cast<int>(y_)},
                                 {static_cast<int>(x_ + (vx_ * 500)),
                                  static_cast<int>(y_ + (vy_ * 500))}};
    SDL_Point circle[17];
    for (int i = 0; i < 17; i++) {
        circle[i].x = x_ + r_ * std::cos(i * 2 * PI / 16);
        circle[i].y = y_ + r_ * std::sin(i * 2 * PI / 16);
    }

    SDL_SetRenderDrawColor(Game::RENDERER, 255, 0, 0, 255);
    SDL_RenderDrawLines(Game::RENDERER, force_vec, 2);
    SDL_RenderDrawLines(Game::RENDERER, circle, 17);
    SDL_SetRenderDrawColor(Game::RENDERER, 255, 255, 0, 255);
    SDL_RenderDrawLines(Game::RENDERER, velocity_vec, 2);
}

double PhysicsObject::getMass() { return m_; }
double PhysicsObject::getPosX() { return x_; }
double PhysicsObject::getPosY() { return y_; }
double PhysicsObject::getVelX() { return vx_; }
double PhysicsObject::getVelY() { return vy_; }
double PhysicsObject::getVelMag() {
    return std::sqrt(std::pow(vx_, 2) + std::pow(vy_, 2));
}
double PhysicsObject::getVelAngle() { return std::atan2(vy_, vx_); }
double PhysicsObject::getAccX() { return ax_; }
double PhysicsObject::getAccY() { return ay_; }
double PhysicsObject::getForceX() { return Fx_; }
double PhysicsObject::getForceY() { return Fy_; }
double PhysicsObject::getKineticEx() { return E_kx_; }
double PhysicsObject::getKineticEy() { return E_ky_; }
double PhysicsObject::getKineticEMag() {
    return std::sqrt(std::pow(E_kx_, 2) + std::pow(E_ky_, 2));
}
