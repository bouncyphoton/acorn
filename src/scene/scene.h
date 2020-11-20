#ifndef ACORN_SCENE_H
#define ACORN_SCENE_H

#include "types.h"
#include "transform.h"
#include "graphics/model.h"
#include "graphics/ibl_probe.h"
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

    void addIblProbe(glm::vec3 position) {
        m_iblProbes.emplace_back(position);
    }

    const std::vector<Geometry> &getGeometry() const {
        return m_geometry;
    }

    std::vector<IblProbe> &getIblProbes() {
        return m_iblProbes;
    }

    const std::vector<IblProbe> &getIblProbes() const {
        return m_iblProbes;
    }

    IblProbe &getDistantIblProbe() {
        return m_distantIblProbe;
    }

    const IblProbe &getDistantIblProbe() const {
        return m_distantIblProbe;
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
    std::vector<IblProbe> m_iblProbes;
    IblProbe m_distantIblProbe;
    Camera m_camera;
    glm::vec3 m_sunDirection;
};

#endif //ACORN_SCENE_H
