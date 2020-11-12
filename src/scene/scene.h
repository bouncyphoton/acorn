#ifndef ACORN_SCENE_H
#define ACORN_SCENE_H

#include "types.h"
#include "transform.h"
#include "graphics/model.h"
#include "camera.h"
#include <vector>

class Geometry {
public:
    Geometry(Transform transform, Model *model)
        : m_transform(transform), m_model(model) {}

    const Transform &getTransform() const {
        return m_transform;
    }

    const Model *getModel() const {
        return m_model;
    }

private:
    Transform m_transform;
    Model *m_model;
};

class Scene {
public:
    Scene();

    void addGeometry(Transform transform, Model *model) {
        m_geometry.emplace_back(transform, model);
    }

    const std::vector<Geometry> &getGeometry() const {
        return m_geometry;
    }

    Camera &getCamera() {
        return m_camera;
    }

    const Camera &getCamera() const {
        return m_camera;
    }

    glm::vec3 getSunDirection() const {
        return m_sunDirection;
    }
private:
    std::vector<Geometry> m_geometry;
    Camera m_camera;
    glm::vec3 m_sunDirection;
};

#endif //ACORN_SCENE_H
