#ifndef ACORN_GRAPHICS_UTIL_H
#define ACORN_GRAPHICS_UTIL_H

#include "types.h"
#include "mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position = glm::vec3(0);
    glm::quat orientation = glm::identity<glm::quat>();
    glm::vec3 scale = glm::vec3(1);
};

struct Material {
    glm::vec3 color = glm::vec3(1);
};

struct Renderable {
    Transform transform;
    Mesh *mesh = nullptr;
    Material *material = nullptr;
};

inline glm::mat4 transform_to_matrix(Transform *transform) {
    if (transform == nullptr) return glm::mat4(1);

    glm::mat4 t = glm::translate(glm::mat4(1), transform->position);
    glm::mat4 r = glm::mat4_cast(transform->orientation);
    glm::mat4 s = glm::scale(glm::mat4(1), transform->scale);

    return t * r * s;
}

#endif //ACORN_GRAPHICS_UTIL_H
