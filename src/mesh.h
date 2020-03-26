#ifndef ACORN_MESH_H
#define ACORN_MESH_H

#include "types.h"
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
//    glm::vec3 tangent;
//    glm::vec3 bitangent;
//    glm::vec2 uv;
};

struct Mesh {
    u32 num_vertices = 0;
    u32 vao = 0;
    u32 vbo = 0;
    Vertex *vertices = nullptr;
};

Mesh mesh_load();

void mesh_free(Mesh *mesh);

#endif //ACORN_MESH_H
