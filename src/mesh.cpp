#include "mesh.h"
#include "core.h"
#include <GL/gl3w.h>

Mesh::Mesh(std::vector<Vertex> vertices, Material material)
        : vertices(vertices), material(material) {
    // Find min and max
    for (auto &v : vertices) {
        min = glm::min(min, v.position);
        max = glm::max(max, v.position);
    }

    init();

    core->debug("Mesh::Mesh(" + std::to_string(vertices.size()) + " vertices, mat) - #" + std::to_string(vao));
}

Mesh::Mesh(Mesh &&other)
        : vao(other.vao),
          vbo(other.vbo),
          vertices(std::move(other.vertices)),
          material(other.material),
          min(other.min),
          max(other.max) {
    other.vao = 0;
    other.vbo = 0;
}

Mesh &Mesh::operator=(Mesh &&other) {
    vao = other.vao;
    vbo = other.vbo;
    vertices = std::move(other.vertices);
    material = other.material;
    min = other.min;
    max = other.max;
    other.vao = 0;
    other.vbo = 0;
    return *this;
}

Mesh::~Mesh() {
    core->debug("Mesh::~Mesh() - " + std::to_string(vao));
    destroy();
}

void Mesh::init() {
    if (vertices.empty()) {
        core->warn("Initializing a mesh with 0 vertices");
    }

    // create vao and vbo for rendering
    glGenVertexArrays(1, &vao);
    if (vao == 0) {
        core->fatal("Failed to generate vao for mesh");
    }
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    if (vbo == 0) {
        core->fatal("Failed to generate vbo for mesh");
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, uv));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, biTangent));

    glBindVertexArray(0);
}

void Mesh::destroy() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    vertices.clear();
}
