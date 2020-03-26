#include "mesh.h"
#include <GL/gl3w.h>

Mesh mesh_load() {
    Mesh mesh = {};
    mesh.num_vertices = 3;
    mesh.vertices = (Vertex *) malloc(sizeof(Vertex) * mesh.num_vertices);

    if (!mesh.vertices) {
        fprintf(stderr, "[error] failed to allocate memory for mesh vertices\n");
        return mesh;
    }

    // set vertices
    mesh.vertices[0] = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)};
    mesh.vertices[1] = {glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)};
    mesh.vertices[2] = {glm::vec3(1, 0, 1), glm::vec3(0, 1, 0)};

    // create vao and vbo for rendering
    glGenVertexArrays(1, &mesh.vao);
    if (mesh.vao == 0) {
        fprintf(stderr, "[error] failed to generate vao for mesh\n");
        return mesh;
    }
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    if (mesh.vbo == 0) {
        fprintf(stderr, "[error] failed to generate vbo for mesh\n");
        return mesh;
    }
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices * sizeof(Vertex), mesh.vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

    glBindVertexArray(0);

    return mesh;
}

void mesh_free(Mesh *mesh) {
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteVertexArrays(1, &mesh->vao);
    free(mesh->vertices);
}
