#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#define DENSITY 0.1f

#include "../blaster.h"
#include "physicsengine.h"
#include <assert.h>
#include <math.h>

///
///@brief The PhysicsObject class
///Each physics object is considered to be a homogeneous circle-shaped blob of
///2D mass.
///
///The time unit used is ticks, and the unit of distance pixel
///Therefore, e.g. the unit for velocity is pixels per ticks.
///
///Basic usage:
///- Inherit a class from PhysicsObject
///- At each frame update, apply forces to the object. Alternatively, you can
///set a constant force to the object.
///- After applying the forces, the physics engine will take a step and
///calculate the new physics for each object
///- You can access the physics parameters only via getter functions. This is to
///limit direct access to the variables and avoid unnatural physics.
///
class PhysicsEngine;
class PhysicsObject {
  public:
    ///
    ///@brief PhysicsObject
    ///@param engine pointer to the physics engine handling all physics objects
    ///@param radius
    ///@param x_initial
    ///@param y_initial
    ///@param allow_out_of_bounds
    ///@param max_speed
    ///
    PhysicsObject(PhysicsEngine *engine, double radius, double x_initial,
                  double y_initial, bool allow_out_of_bounds,
                  double max_speed = -1.0f, bool friction_decay = true);
    ~PhysicsObject();

    ///
    ///@brief Take a step in time and calculate new object physics. SHOULD BE
    ///ONLY CALLED BY THE PHYSICS ENGINE
    ///
    void calculatePhysics(double friction_decay, bool visual_debug);

    ///
    ///@brief setRadius
    ///
    void setRadius(double radius);

    ///
    ///@brief Set the speed vector of the object to a known value.
    ///Note that this will be changed as soon as a force is applied to the
    ///object. It is therefore recommended to only use this function when
    ///spawning a new object, to avoid unnatural physics behaviour.
    ///@param angle_rad
    ///@param magnitude
    ///
    void setSpeed(double angle_rad, double magnitude);

    ///
    ///@brief Set the speed vector of the object to a known value.
    ///Note that this will be changed as soon as a force is applied to the
    ///object. It is therefore recommended to only use this function when
    ///spawning a new object, to avoid unnatural physics behaviour.
    ///@param angle_rad
    ///@param magnitude
    ///
    void setSpeedXY(double vx, double vy);

    ///
    ///@brief Set object maximum speed limit
    ///@param max_speed
    ///
    void setMaxSpeed(double max_speed);

    ///
    ///@brief Add a force vector to the object sum force vector
    ///
    void addForce(double angle_rad, double magnitude);

    ///
    ///@brief Add a force vector which points to a given coordinate point
    ///@param x
    ///@param y
    ///
    void addForceToPoint(double x, double y, double magnitude);

    ///
    ///@brief Set a constant force which will not be resetted at each physics
    ///step
    ///@param angle_rad
    ///@param magnitude
    ///
    void setConstantForce(double angle_rad, double magnitude);

    ///
    ///@brief Calculate collision physics with another physics object
    ///@return Collision impulse
    ///
    double physicsCollisionWith(PhysicsObject *obj);

    ///
    /// \brief Checks if the object is moving towards another
    /// \param obj PhysicsObject to check against
    /// \return true if the objects are approaching each other
    ///
    bool isApproaching(PhysicsObject *obj);

    ///
    ///@brief Reset physics state by setting the position and resetting other
    ///physics variables
    ///
    void resetPhysicsState(double x, double y);

    ///
    ///@brief Render debug graphics for objects physics
    ///
    void renderDebugPhysics();

  public: // these are aimed for the use in inherited classes, as we'll avoid
          // giving direct access to the physics variables
    double getMass();
    double getPosX();
    double getPosY();
    double getVelX();
    double getVelY();
    double getVelMag();
    double getVelAngle();
    double getAccX();
    double getAccY();
    double getForceX();
    double getForceY();
    double getKineticEx();
    double getKineticEy();
    double getKineticEMag();

  private:            // physics variables
    double r_;        // object radius
    double m_;        // mass
    double x_;        // x position
    double y_;        // y position
    double vx_;       // x velocity
    double vy_;       // y velocity
    double ax_;       // x acceleration
    double ay_;       // y acceleration
    double Fx_;       // x force vector
    double Fy_;       // y force vector
    double Fx_const_; // constant x force vector
    double Fy_const_; // constant y force vector
    double E_kx_;     // x kinetic energy
    double E_ky_;     // x kinetic energy

  protected: // other protected variables
    PhysicsEngine *physicsEngine_;
    bool allow_out_of_bounds_;
    bool friction_decay_;
    double max_speed_;
    Uint32 ticks_;

  private: // functions handled by PhysicsObject class only
    void checkBounds();
};

#endif // PHYSICSOBJECT_H
