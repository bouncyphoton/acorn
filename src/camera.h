#ifndef ACORN_CAMERA_H
#define ACORN_CAMERA_H

#include "types.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// TODO (eventually): physically based camera

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

    f32 getNearPlane() const {
        return m_nearPlane;
    }

    void setNearPlane(f32 near_plane) {
        m_nearPlane = near_plane;
    }

    f32 getFarPlane() const {
        return m_farPlane;
    }

    void setFarPlane(f32 far_plane) {
        m_farPlane = far_plane;
    }

private:
    glm::vec3 m_position = glm::vec3(0);
    glm::vec3 m_lookPosition = glm::vec3(0, 0, -1);
    f32 m_fov = glm::half_pi<f32>();
    f32 m_exposure = 1.0f;
    f32 m_nearPlane = 0.01f;
    f32 m_farPlane = 1000.0f;
};

#endif //ACORN_CAMERA_H
