#include "collisionutils.h"
#include "entity.h"


inline void checkHit(Entity* e1, Entity* e2) {
    if (e1->getBody()->intersects(e2->getBody())) {
        e1->collisionWith(e2);
        e2->collisionWith(e1);
    }
}

void runCollisions() {
    auto entities = Entity::getEntities();
    auto it = entities.begin();

    while (it != entities.end()) {
        auto e1 = *it;

        // Skip non-collidables
        if (!e1->isCollidable()) {
            ++it;
            continue;
        }

        auto it2 = it;
        it2++;
        while (it2 != entities.end()) {
            auto e2 = *it2;

            // Skip non-collidables
            if (!e2->isCollidable()) {
                ++it2;
                continue;
            }

            // Directly skip entities far away
            if (!e1->getBody()->isCloseTo(e2->getBody())) {
                ++it2;
                continue;
            }

            auto e1_type = e1->getType();
            auto e2_type = e2->getType();

            // Check if these types can collide with each other
            if (!e1->doesCollideWith(e2_type) || !e2->doesCollideWith(e1_type)) {
                ++it2;
                continue;
            }

            checkHit(e1, e2);
            ++it2;
        }
        ++it;
    }
}
