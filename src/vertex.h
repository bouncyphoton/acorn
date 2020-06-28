#ifndef ACORN_VERTEX_H
#define ACORN_VERTEX_H

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
};

#endif //ACORN_VERTEX_H
