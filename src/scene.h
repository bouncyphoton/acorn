#ifndef ACORN_SCENE_H
#define ACORN_SCENE_H

#include "types.h"
#include "entity.h"
#include <unordered_map>

class Scene {
public:
    u32 addEntity(Entity entity);

    void removeEntity(entityHandle_t handle);

    // TODO: add the ability to update selective parts of entity
    void updateEntity(entityHandle_t handle, Entity entity);

    const std::vector<Entity> &getEntities() const;

    // TODO: scene "globaL" properties

    // unit vector pointing towards the sun
    glm::vec3 sunDirection = glm::vec3(0, 1, 0);

private:
    // TODO: spacial partitioning
    std::vector<Entity> m_entities;

    std::vector<entityHandle_t> m_unusedIndices;
};

#endif //ACORN_SCENE_H
