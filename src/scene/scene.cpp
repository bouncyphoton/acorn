#include "scene.h"

Scene::Scene()
    : m_camera(), m_sunDirection(glm::normalize(glm::vec3(1))), m_distantIblProbe(glm::vec3(0)) {
    // TODO: generate a sky or something for distant probe
    // TODO: also needs to be processed
    m_distantIblProbe.clearToWhite();
}
