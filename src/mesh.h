#ifndef ACORN_MESH_H
#define ACORN_MESH_H

#include "types.h"
#include "material.h"
#include "vertex.h"
#include <glm/glm.hpp>
#include <vector>

class Mesh {
public:
    Mesh(const std::vector<Vertex> &vertices, Material material);
    Mesh(Mesh &&other);
    Mesh &operator=(Mesh &&other);
    ~Mesh();

    void draw() const;

    const Material &getMaterial() const {
        return m_material;
    }

    u32 getNumVertices() const {
        return m_numVertices;
    }

private:
    u32 m_vao = 0;
    u32 m_vbo = 0;
    u32 m_numVertices = 0;
    Material m_material;
    glm::vec3 m_min = glm::vec3(INFINITY);
    glm::vec3 m_max = glm::vec3(-INFINITY);
};

#endif //ACORN_MESH_H
