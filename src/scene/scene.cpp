#include "scene.h"

Scene::Scene()
    : m_camera(), m_sunDirection(glm::normalize(glm::vec3(1))) {}
