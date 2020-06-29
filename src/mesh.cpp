#include "mesh.h"
#include "core.h"
#include <GL/gl3w.h>

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
