#include "scene.h"

u32 Scene::addEntity(Entity entity) {
    u32 handle;
    if (!m_unusedIndices.empty()) {
        handle = m_unusedIndices.back();
        m_unusedIndices.pop_back();
        m_entities[handle] = entity;
    } else {
        handle = m_entities.size();
        m_entities.emplace_back(entity);
    }
    return handle;
}

void Scene::removeEntity(entityHandle_t handle) {
    m_entities[handle].active = false;
    m_unusedIndices.emplace_back(handle);
}

void Scene::updateEntity(entityHandle_t handle, Entity entity) {
    m_entities[handle] = entity;
}

const std::vector<Entity> &Scene::getEntities() const {
    return m_entities;
}
