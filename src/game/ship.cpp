#include "ship.h"
#include "../game.h"
#include <stdexcept>

Ship::~Ship() {}
Ship::Ship(PhysicsEngine *physicsEngine,
           RenderEngine *renderEngine,
           ParticleHandler *particleHandler,
           int x_initial,
           int y_initial,
           double initial_angle,
           int h,
           int w)
    : PhysicsObject(physicsEngine, h, x_initial, y_initial, false, SHIP_MAX_SPEED),
      Entity(true, SHIP),
      particleHandler_(particleHandler),
      renderEngine_(renderEngine) {

    // Set collision properties
    setCollidable(true);
    ENABLE_COLLISION(SHIP);
    ENABLE_COLLISION(ASTEROID);
    ENABLE_COLLISION(BULLET);
    LOG("%d", getId());
    weapons_ = {
        Weapon::Ptr(new Cannon(renderEngine_, physicsEngine_, this)),
        Weapon::Ptr(new MachineGun(renderEngine_, physicsEngine_, this)),
        Weapon::Ptr(new DoubleShotgun(renderEngine_, physicsEngine_, this)),
        Weapon::Ptr(new MiniNuke(renderEngine_, physicsEngine_, this))};

    activeWeapon_ = weapons_[0];

    // Ship status
    lives_ = SHIPLIVES;
    alive = true;
    height_ = h;
    width_ = w;
    heading_rad_ = initial_angle * DEG2RAD;
    spawn_cooldown_ = SDL_GetTicks();

    // Create ship chassis
    body.setRenderLayer(TOP_RENDER_LAYER_IDX);
    body.init(renderEngine,
              calculateChassis_(x_initial, y_initial),
              x_initial, y_initial);
    body.setRenderType(FILL);
    body.setColor(SDL_Color{255, 255, 255, 255});
    top_corner_idx_ = 0;
    bottom_right_corner_idx_ = 2;
    bottom_left_corner_idx_ = 3;
}

std::vector<SDL_Point> Ship::calculateChassis_(double x, double y) {

    // chassis calculation has this weird property when it comes to the ship
    // direction, fix pls?
    double a_cos = cos(heading_rad_ - PI / 2);
    double a_sin = sin(heading_rad_ - PI / 2);

    std::vector<SDL_Point> outline;

    // Corners of the chassis
    int xtop_ = static_cast<int>(-(height_)*a_sin + x);
    int ytop_ = static_cast<int>((height_)*a_cos + y);

    int xleft_ = static_cast<int>(-(width_)*a_cos - (-height_) * a_sin + x);
    int yleft_ = static_cast<int>(-(width_)*a_sin + (-height_) * a_cos + y);

    int xright_ = static_cast<int>((width_)*a_cos - (-height_) * a_sin + x);
    int yright_ = static_cast<int>((width_)*a_sin + (-height_) * a_cos + y);

    int xleft_m_ =
        static_cast<int>(-(width_)*a_cos - (-height_ * 0.3f) * a_sin + x);
    int yleft_m_ =
        static_cast<int>(-(width_)*a_sin + (-height_ * 0.3f) * a_cos + y);

    int xright_m_ =
        static_cast<int>((width_)*a_cos - (-height_ * 0.3f) * a_sin + x);
    int yright_m_ =
        static_cast<int>((width_)*a_sin + (-height_ * 0.3f) * a_cos + y);

    outline.push_back(SDL_Point{xtop_, ytop_});
    outline.push_back(SDL_Point{xleft_m_, yleft_m_});
    outline.push_back(SDL_Point{xleft_, yleft_});
    outline.push_back(SDL_Point{xright_, yright_});
    outline.push_back(SDL_Point{xright_m_, yright_m_});
    outline.push_back(SDL_Point{xtop_, ytop_});

    return outline;
}

void Ship::reset(int x_initial, int y_initial, double initial_angle) {
    // Reset ship status
    lives_ = SHIPLIVES;
    alive = true;
    spawn_cooldown_ = SDL_GetTicks();
    body.setRendering(true);

    // Reset ship position
    resetPhysicsState(x_initial, y_initial);

    // Rotate body back to starting position
    body.rotate((initial_angle * DEG2RAD) - heading_rad_);
    heading_rad_ = initial_angle * DEG2RAD;

    // Reset weaponry
    for (auto weapon : weapons_) {
        weapon->reset();
    }
}

void Ship::rotate(Rotation direction) {

    double rot = ROTATION_SPEED * DEG2RAD * g_timescale;

    switch (direction) {
    case Clockwise:
        heading_rad_ = (heading_rad_ + rot);
        body.rotate(rot);
        break;
    case Anticlockwise:
        heading_rad_ = (heading_rad_ - rot);
        body.rotate(-rot);
        break;
    }
}

void Ship::accelerate() {
    addForce(heading_rad_, THRUST_FORCE);
    createParticles_();
}

void Ship::createParticles_() {

    double opp_direction_rad = heading_rad_ + PI;

    SDL_Point bottom_right = body.outline[bottom_right_corner_idx_];
    SDL_Point bottom_left = body.outline[bottom_left_corner_idx_];

    int x_exhaust = (bottom_left.x + bottom_right.x) / 2;
    int y_exhaust = (bottom_left.y + bottom_right.y) / 2;

    int amount_outer = 30 * g_timescale;
    int amount_inner = 15 * g_timescale;

    particleHandler_->createParticleBurst(
        opp_direction_rad, particleSpeed_, x_exhaust, y_exhaust, getVelX(),
        getVelY(), amount_outer, 0.6f, 130, SDL_Color{255, 180, 10, 255});

    particleHandler_->createParticleBurst(
        opp_direction_rad, particleSpeed_, x_exhaust, y_exhaust, getVelX(),
        getVelY(), amount_inner, 0.3f, 80, SDL_Color{255, 255, 10, 255});
}

void Ship::updateTimeAlive_() {
    Uint32 ticks = SDL_GetTicks();
    if ((ticks - time_alive_change_) > 1000) {
        time_alive_++;
        time_alive_change_ = ticks;
    }
}

void Ship::reload() {
    activeWeapon_->reload();
}

void Ship::shoot(BulletHandler *bHandler) {
    // Spawn cooldown (500ms)
    if ((SDL_GetTicks() - spawn_cooldown_) < 500)
        return;

    SDL_Point top = body.outline[top_corner_idx_];
    activeWeapon_->shoot(bHandler, particleHandler_, heading_rad_, top.x,
                         top.y);
}

void Ship::switchWeapon(unsigned long slot) {
    if (weapons_.size() > slot) {
        activeWeapon_->stopReloading();
        activeWeapon_ = weapons_[slot];
    }
}

void Ship::update() {
    damage_taken_ = false;
    // Time alive
    if (alive) {
        updateTimeAlive_();
        // Calculate ship chassis using the new location
        body.moveAbsolute(getPosX(), getPosY());
    }
}

void Ship::collision() {
    damage_taken_ = true;
    if (alive) {
        lives_--;
        if (lives_ == 0) {
            alive = false;
            body.setRendering(false);
            particleHandler_->createParticleExplosion(
                getPosX(), getPosY(), DEFAULT_EXPLOSION_SPEED, getVelX(),
                getVelY(), 200, DEFAULT_PARTICLE_LIFESPAN);
        }
    }
}

void Ship::collisionWith(Entity *e) {
    // Self-collisions not allowed
        switch (e->getType()) {
        case ASTEROID:
            collision();
            break;
        case BULLET:
            if (e->getOwnerId() != getId())
                collision();
            break;
        }
}

Polygon* Ship::getBody() {
    return &body;
}

// Getters
Weapon* Ship::getActiveWeapon() { return activeWeapon_.get(); }
unsigned int Ship::getTimeAlive() { return time_alive_; }
int Ship::getLives() { return lives_; }

/// PROTOTYPE for multiplayer purposes
std::vector<std::string> Ship::getOutline_() {
    std::vector<std::string> out;

    for (int i = 0; i < 6; ++i) {
        out.push_back(std::to_string(body.outline[i].x));
        out.push_back(std::to_string(body.outline[i].y));
    }

    return out;
}
bool Ship::isDamageTaken() const { return damage_taken_; }

