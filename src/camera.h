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

    glm::vec3 getForward() const;

    glm::vec3 getRight() const;

    glm::vec3 getPosition() const {
        return m_position;
    }

    void setPosition(glm::vec3 position) {
        m_position = position;
    }

    void addPosition(glm::vec3 position) {
        setPosition(getPosition() + position);
    }

    glm::vec2 getLookRotation() const {
        return m_rotation;
    }

    void setLookRotation(glm::vec2 rotation) {
        f32 epsilon = 0.01f;
        rotation.y = glm::clamp(rotation.y, -glm::half_pi<f32>() + epsilon, glm::half_pi<f32>() - epsilon);
        rotation.x = glm::mod(rotation.x, glm::two_pi<f32>());

        m_rotation = rotation;
    }

    void addLookRotation(glm::vec2 rotation) {
        setLookRotation(getLookRotation() + rotation);
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

    constexpr static glm::vec3 UP = glm::vec3(0, 1, 0);

private:
    glm::vec3 m_position = glm::vec3(0);
    glm::vec2 m_rotation = glm::vec2(0);
    f32 m_fov = glm::half_pi<f32>();
    f32 m_exposure = 1.0f;
    f32 m_nearPlane = 0.01f;
    f32 m_farPlane = 1000.0f;
};

#endif //ACORN_CAMERA_H
