#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "physicsobject.h"
#include <set>

const static double FRICTION_DECAY = 0.005;

///
///@brief The PhysicsEngine handles all PhysicsObjects by calculating their
/// physics when the engine function "step" is called
///
class PhysicsObject;
class PhysicsEngine {
  public:
    PhysicsEngine();

    void addObject(PhysicsObject *obj);
    void removeObject(PhysicsObject *obj);
    void step();

  private:
    std::set<PhysicsObject *> objects_;
};

#endif // PHYSICSENGINE_H
