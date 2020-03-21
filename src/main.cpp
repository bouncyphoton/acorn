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

    Transform transform = {};
    transform.position = glm::vec3(-0.5f, 0.5f, -1);
    transform.orientation = glm::quat(glm::vec3(glm::half_pi<f32>(), 0, 0));

    Mesh mesh = {};
    mesh.num_vertices = 3;
    mesh.vertices = (Vertex *) malloc(sizeof(Vertex) * mesh.num_vertices);
    mesh.vertices[0] = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)};
    mesh.vertices[1] = {glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)};
    mesh.vertices[2] = {glm::vec3(1, 0, 1), glm::vec3(0, 1, 0)};

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.num_vertices * sizeof(Vertex), mesh.vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

    glBindVertexArray(0);

    Material material = {};
    material.color = glm::vec3(1, 0.5, 1);

    Renderable obj = {};
    obj.transform = transform;
    obj.mesh = &mesh;
    obj.material = &material;

    while (!window_should_close()) {
        window_update();

        renderer_draw(&obj, 1);

        window_swap_buffers();
    }

    // TODO: delete
    free(mesh.vertices);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &mesh.vao);
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
