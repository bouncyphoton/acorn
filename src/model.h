#ifndef ACORN_MODEL_H
#define ACORN_MODEL_H

#include "material.h"
#include "types.h"
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
};

struct Mesh {
    u32 num_vertices = 0;
    u32 vao = 0;
    u32 vbo = 0;
    Vertex *vertices = nullptr;
    glm::vec3 min = glm::vec3(INFINITY);
    glm::vec3 max = glm::vec3(-INFINITY);
};

struct Model {
    u32 num_meshes = 0;
    Material *materials = nullptr;
    Mesh *meshes = nullptr;
};

Model model_load_from_obj(const char *obj_path, const char *mtl_dir);

void model_free(Model *model);

#endif //ACORN_MODEL_H
