#include "camera.h"
#include "core.h"

glm::mat4 Camera::getViewProjectionMatrix() const {
    static constexpr glm::vec3 UP = glm::vec3(0, 1, 0);

    // TODO: no need to compute this if position etc. hasn't changed

    // NOTE: currently, camera is tied to render width and height
    f32 aspectRatio = (f32) core->gameState.renderOptions.width / core->gameState.renderOptions.height;

    glm::mat4 viewMatrix = glm::lookAt(m_position, m_lookPosition, UP);
    glm::mat4 projectionMatrix = glm::perspective(m_fov, aspectRatio, m_nearPlane, m_farPlane);
    return projectionMatrix * viewMatrix;
}
