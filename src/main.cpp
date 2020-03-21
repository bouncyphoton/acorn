#include "window.h"
#include "renderer.h"
#include <GL/gl3w.h>
#include <cstdio>

static bool acorn_init() {
    u32 window_width = 800;
    u32 window_height = 600;
    const char *window_title = "acorn";

    if (!window_init(window_width, window_height, window_title)) return false;
    if (!renderer_init()) return false;

    puts("[info] acorn successfully initialized");

    return true;
}

static void acorn_shutdown() {
    puts("[info] shutting down acorn");

    renderer_shutdown();
    window_shutdown();
}

static void acorn_run() {
    // TODO: remove test renderable

    transform mesh_transform = {};
    mesh_transform.position = glm::vec3(-0.5f, 0.5f, -1);
    mesh_transform.orientation = glm::quat(glm::vec3(glm::half_pi<f32>(), 0, 0));

    mesh mesh_data = {};
    mesh_data.num_vertices = 3;
    mesh_data.vertices = (vertex *) malloc(sizeof(vertex) * mesh_data.num_vertices);
    mesh_data.vertices[0] = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)};
    mesh_data.vertices[1] = {glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)};
    mesh_data.vertices[2] = {glm::vec3(1, 0, 1), glm::vec3(0, 1, 0)};

    glGenVertexArrays(1, &mesh_data.vao);
    glBindVertexArray(mesh_data.vao);

    glGenBuffers(1, &mesh_data.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_data.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh_data.num_vertices * sizeof(vertex), mesh_data.vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (const void *) offsetof(vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (const void *) offsetof(vertex, normal));

    glBindVertexArray(0);

    material mesh_material = {};
    mesh_material.color = glm::vec3(1, 0.5, 1);

    renderable obj = {};
    obj.mesh_transform = mesh_transform;
    obj.mesh_data = &mesh_data;
    obj.mesh_material = &mesh_material;

    while (!window_should_close()) {
        window_update();

        renderer_draw(&obj, 1);

        window_swap_buffers();
    }

    // TODO: delete
    free(mesh_data.vertices);
    glDeleteBuffers(1, &mesh_data.vbo);
    glDeleteVertexArrays(1, &mesh_data.vao);
}

int main() {
    // run if initialized successfully
    if (acorn_init()) {
        acorn_run();
    }

    // shutdown
    acorn_shutdown();

    return 0;
}
