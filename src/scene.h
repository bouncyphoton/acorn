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

private:
    // TODO: spacial partitioning
    std::vector<Entity> m_entities;

    std::vector<entityHandle_t> m_unusedIndices;
};

#endif //ACORN_SCENE_H
