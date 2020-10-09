#include "mesh.h"
#include "core.h"
#include <GL/gl3w.h>

Mesh::Mesh(const std::vector<Vertex> &vertices, Material material)
    : m_numVertices(vertices.size()), m_material(material) {
    // Find min and max
    for (auto &v : vertices) {
        m_min = glm::min(m_min, v.position);
        m_max = glm::max(m_max, v.position);
    }

    if (vertices.empty()) {
        core->warn("Initializing a mesh with 0 vertices");
    }

    // create vao and vbo for rendering
    glGenVertexArrays(1, &m_vao);
    if (m_vao == 0) {
        core->fatal("Failed to generate vao for mesh");
    }

    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    if (m_vbo == 0) {
        core->fatal("Failed to generate vbo for mesh");
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
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

    core->debug("Mesh::Mesh(" + std::to_string(vertices.size()) + " vertices, mat) - #" + std::to_string(m_vao));
}

Mesh::Mesh(Mesh &&other) noexcept
    : m_vao(other.m_vao),
      m_vbo(other.m_vbo),
      m_numVertices(other.m_numVertices),
      m_material(other.m_material),
      m_min(other.m_min),
      m_max(other.m_max) {
    other.m_vao = 0;
    other.m_vbo = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_numVertices = other.m_numVertices;
    m_material = other.m_material;
    m_min = other.m_min;
    m_max = other.m_max;
    other.m_vao = 0;
    other.m_vbo = 0;
    return *this;
}

Mesh::~Mesh() {
    core->debug("Mesh::~Mesh() - " + std::to_string(m_vao));
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}
