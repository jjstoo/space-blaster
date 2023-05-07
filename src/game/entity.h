#ifndef ENTITY_H
#define ENTITY_H
#include "graphics.h"
#include <list>
#include <bitset>

/// Enables collisions against given entity type
#define ENABLE_COLLISION(e) collidesWith_[e] = true

class Ship;
class Asteroid;
class Bullet;
enum EntityType {
    SHIP,
    ASTEROID,
    BULLET,
    UNDEFINED,
    _entity_type_max
};

///
/// \class Entity represents any game object with the possibility to interact
/// with other objects. Entities can act on their own or be owned by other
/// entities.
///
/// Entities also represent remote multiplayer objects. Therefore all attributes
/// relevant to multiplayer (score, for example) should be encapsulated inside
/// the Entity class instead of inheriting classes.
///
/// Plan is to have the complete state of each entity to be serialized and shared
/// with every multiplayer participant, resulting in a common entity pool.
///
class Entity;
typedef std::list<Entity*> EntityList;
class Entity
{
    static const int ENTITY_MAX_ID = 25600000;
public:
    ~Entity();

    /// Constructs an entity
    /// \param identifiable whether to generate an id for the entity
    /// \param type entity type
    explicit Entity(bool identifiable, EntityType type = UNDEFINED);

    /// Constructs an entity with an owner bound by id
    /// \param owner_id owner id
    /// \param identifiable whether to generate an id for the entity
    /// \param type entity type
    Entity(int owner_id, bool identifiable, EntityType type = UNDEFINED);

    /// Constructs an entity with an owner
    /// \param owner owner entity
    /// \param identifiable whether to generate an id for the entity
    /// \param type entity type
    Entity(Entity* owner, bool identifiable, EntityType type = UNDEFINED);


    /// Get vector of active entities
    /// \return
    static EntityList getEntities();

    /// Checks if the entity can collide with other entities of the given type
    /// \param t entity type to check against
    /// \return true if the entity can collide
    bool doesCollideWith(EntityType t);

    /// Checks if an id is set for the entity
    /// \return true if an id is set
    bool hasId();

    /// Checks if the entity has an owner
    /// \return true if the entity has an owner
    bool hasOwner();

    /// Checks if the entity is collidable
    /// \return true if the entity is collidable
    [[nodiscard]] bool isCollidable() const;

    /// Gets the entities id
    /// \return entity id
    [[nodiscard]] int getId() const;

    /// Gets the entities owners id
    /// \return owner id, -1 if not set
    [[nodiscard]] int getOwnerId() const;

    /// Gets the entities owner
    /// \return entity owner, NULL if not set
    Entity* getOwner();

    /// Gets the entity type
    /// \return entity type
    EntityType getType();

    /// Sets the entities owner
    /// \param owner target owner
    void setOwner(Entity* owner);

    /// Sets the entities collidability
    /// \param collidable whether the entity is collidable
    void setCollidable(bool collidable);

    /// Get score
    /// \return score
    [[nodiscard]] int getScore() const;

    /// Add points to ships score
    /// \param points points to add
    void addToScore(int points);

    /// Check if the instance is a remote object
    /// \return true if the object was not created locally
    [[nodiscard]] bool isRemote() const;

    /// Set the objects remote state
    /// \param remote remote state
    void setRemote(bool remote);

    /// Set a body for the Entity
    /// \param body target body
    void setBody(const Polygon::Ptr &body);

    // Virtuals
    /// Returns the body of the entity, must be implemented if the entity
    /// is collidable
    /// \return entities body
    virtual Polygon* getBody() = 0;

    /// Handles a collision with a given entity, must be implemented if the
    /// entity is collidable
    /// \param e other party of the collision
    virtual void collisionWith(Entity* e) = 0;

protected:
    static EntityList entities_;
    EntityType type_ = UNDEFINED;
    Entity* owner_ = nullptr;
    std::bitset<_entity_type_max> collidesWith_;
    Polygon::Ptr body_;

    int id_ = -1;
    int owner_id_ = -1;
    int score_ = 0;

    bool remote_ = false;
    bool identifiable_;
    bool owned_;
    bool collidable_;

};
#endif // ENTITY_H
