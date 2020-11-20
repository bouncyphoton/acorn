#include "scene.h"

Scene::Scene()
    : m_camera(), m_sunDirection(glm::normalize(glm::vec3(1))), m_distantIblProbe(glm::vec3(0)) {
    m_distantIblProbe.clearToWhite();
}
