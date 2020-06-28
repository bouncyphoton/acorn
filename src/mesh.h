#ifndef ACORN_MESH_H
#define ACORN_MESH_H

#include "types.h"
#include "material.h"
#include "vertex.h"
#include <glm/glm.hpp>
#include <vector>

struct Mesh {
    void init();

    void destroy();

    u32 vao = 0;
    u32 vbo = 0;
    std::vector<Vertex> vertices;
    Material material;
    glm::vec3 min = glm::vec3(INFINITY);
    glm::vec3 max = glm::vec3(-INFINITY);
};

#endif //ACORN_MESH_H
