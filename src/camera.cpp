#include "camera.h"
#include "core.h"

constexpr glm::vec3 Camera::UP;

glm::mat4 Camera::getViewProjectionMatrix() const {
    // TODO: no need to compute this if position etc. hasn't changed

    static const ConfigData &config = core->config.getConfigData();

    // NOTE: currently, camera is tied to render width and height
    f32 aspectRatio = (f32) config.width / config.height;

    glm::mat4 viewMatrix = glm::lookAt(m_position, m_position + getForward(), Camera::UP);
    glm::mat4 projectionMatrix = glm::perspective(m_fov, aspectRatio, m_nearPlane, m_farPlane);
    return projectionMatrix * viewMatrix;
}

glm::vec3 Camera::getForward() const {
    return glm::vec3(cos(m_rotation.x) * cos(m_rotation.y),
                     sin(m_rotation.y),
                     sin(m_rotation.x) * cos(m_rotation.y));
}

glm::vec3 Camera::getRight() const {
    return glm::normalize(glm::cross(getForward(), Camera::UP));
}
