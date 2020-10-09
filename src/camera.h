#ifndef ACORN_CAMERA_H
#define ACORN_CAMERA_H

#include "types.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Camera {
public:
    Camera() = default;

    glm::mat4 getViewProjectionMatrix() const;

    glm::vec3 getPosition() const {
        return m_position;
    }

    void setPosition(glm::vec3 position) {
        m_position = position;
    }

    glm::vec3 getLookPosition() const {
        return m_lookPosition;
    }

    void setLookPosition(glm::vec3 look_position) {
        m_lookPosition = look_position;
    }

    f32 getFov() const {
        return m_fov;
    }

    void setFov(f32 fov) {
        m_fov = fov;
    }

    f32 getExposure() const {
        return m_exposure;
    }

    void setExposure(f32 exposure) {
        m_exposure = exposure;
    }

private:
    glm::vec3 m_position = glm::vec3(0);
    glm::vec3 m_lookPosition = glm::vec3(0, 0, -1);
    f32 m_fov = glm::half_pi<f32>();
    f32 m_exposure = 1.0f;
};

#endif //ACORN_CAMERA_H
