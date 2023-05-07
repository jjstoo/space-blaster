#include "physicsengine.h"

PhysicsEngine::PhysicsEngine() {}

void PhysicsEngine::addObject(PhysicsObject *obj) { objects_.insert(obj); }

void PhysicsEngine::removeObject(PhysicsObject *obj) { objects_.erase(obj); }

void PhysicsEngine::step() {
    for (auto obj : objects_) {
        obj->calculatePhysics(FRICTION_DECAY, PHYSICS_VISUAL_DEBUG);
    }
}
