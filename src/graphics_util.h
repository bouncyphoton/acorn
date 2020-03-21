#ifndef ACORN_GRAPHICS_UTIL_H
#define ACORN_GRAPHICS_UTIL_H

#include "types.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct transform {
    glm::vec3 position = glm::vec3(0);
    glm::quat orientation = glm::identity<glm::quat>();
    glm::vec3 scale = glm::vec3(1);
};

struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
//    glm::vec3 tangent;
//    glm::vec3 bitangent;
//    glm::vec2 uv;
};

struct mesh {
    u32 num_vertices = 0;
    u32 vao = 0;
    u32 vbo = 0;
    vertex *vertices = nullptr;
};

struct material {
    glm::vec3 color = glm::vec3(1);
};

struct renderable {
    transform mesh_transform;
    mesh *mesh_data = nullptr;
    material *mesh_material = nullptr;
};

inline glm::mat4 transform_to_matrix(transform *transform) {
    if (transform == nullptr) return glm::mat4(1);

    glm::mat4 t = glm::translate(glm::mat4(1), transform->position);
    glm::mat4 r = glm::mat4_cast(transform->orientation);
    glm::mat4 s = glm::scale(glm::mat4(1), transform->scale);

    return t * r * s;
}

#endif //ACORN_GRAPHICS_UTIL_H
