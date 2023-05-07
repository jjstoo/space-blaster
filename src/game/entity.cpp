#include "entity.h"
#include "rng.h"

EntityList Entity::entities_;

Entity::~Entity() {
    entities_.remove(this);
}

Entity::Entity(bool identifiable, EntityType type)
{
    type_ = type;
    identifiable_ = identifiable;
    owned_ = false;
    if (identifiable) {
        // Collisions possible but let's take the risk
        id_ = random_int_in_range<int>(0, ENTITY_MAX_ID, UNIFORM);
    } else {
        id_ = -1;
    }

    entities_.push_back(this);
}

Entity::Entity(int owner_id, bool identifiable, EntityType type) :
    Entity(identifiable, type) {
    owner_id_ = owner_id;
    owned_ = true;
}

Entity::Entity(Entity* owner, bool identifiable, EntityType type) :
    Entity(identifiable, type) {
    owner_ = owner;
    owner_id_ = owner->getId();
    owned_ = true;
}

void Entity::setOwner(Entity *owner) {
    if (!owner) {
        owned_ = false;
        owner_id_ = -1;
    }
    owner_ = owner;
    owned_ = true;
    owner_id_ = owner->getId();
}

void Entity::collisionWith(Entity*) {
    // Virtual function
}

Polygon * Entity::getBody() {
    return body_.get();
}

void Entity::setCollidable(bool collidable) { collidable_ = collidable; }
EntityList Entity::getEntities() { return entities_; }
bool Entity::hasId() { return identifiable_; }
bool Entity::hasOwner() { return owned_; }
bool Entity::isCollidable() const { return collidable_; }
int Entity::getId() const { return id_; }
int Entity::getOwnerId() const { return owner_id_; }
Entity* Entity::getOwner() { return owner_; }
EntityType Entity::getType() { return type_; }
bool Entity::doesCollideWith(EntityType t) { return collidesWith_[t]; }
int Entity::getScore() const { return score_; }
void Entity::addToScore(int points) { score_ += points; }
bool Entity::isRemote() const { return remote_; }
void Entity::setRemote(bool remote) { remote_ = remote; }
void Entity::setBody(const Polygon::Ptr &body) { body_ = body; }
