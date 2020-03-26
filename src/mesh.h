#ifndef ACORN_MESH_H
#define ACORN_MESH_H

#include "types.h"
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
//    glm::vec3 tangent;
//    glm::vec3 bitangent;
//    glm::vec2 uv;
};

struct Mesh {
    u32 num_vertices = 0;
    u32 vao = 0;
    u32 vbo = 0;
    Vertex *vertices = nullptr;
    glm::vec3 min = glm::vec3(INFINITY);
    glm::vec3 max = glm::vec3(-INFINITY);
};

Mesh mesh_load(const char *obj_path, const char *mtl_dir);

void mesh_free(Mesh *mesh);

#endif //ACORN_MESH_H
