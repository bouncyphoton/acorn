#include "scene.h"

u32 Scene::addEntity(Entity entity) {
    u32 id;
    if (!m_unusedIndices.empty()) {
        id = m_unusedIndices.back();
        m_unusedIndices.pop_back();
        m_entities[id] = entity;
    } else {
        id = m_entities.size();
        m_entities.emplace_back(entity);
    }
    return id;
}

void Scene::removeEntity(entityHandle_t handle) {
    m_unusedIndices.emplace_back(handle);
}

void Scene::updateEntity(entityHandle_t handle, Entity entity) {
    m_entities[handle] = entity;
}
