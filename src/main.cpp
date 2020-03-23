#include "window.h"
#include "renderer.h"
#include <GL/gl3w.h>
#include <cstdio>

#define NUM_MESHES 1
#define NUM_MATERIALS 1
// TODO: move meshes and materials data, preferably to a memory arena
static Mesh meshes[NUM_MESHES] = {};
static Material materials[NUM_MATERIALS] = {};

bool assets_load() {
    // load material
    materials[0].color = glm::vec3(1, 0.5, 1);

    // load mesh
    Mesh *mesh = &meshes[0];
    mesh->num_vertices = 3;
    mesh->vertices = (Vertex *) malloc(sizeof(Vertex) * mesh->num_vertices);

    if (!mesh->vertices) {
        fprintf(stderr, "[error] failed to allocate memory for mesh\n");
        return false;
    }

    // set vertices
    mesh->vertices[0] = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)};
    mesh->vertices[1] = {glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)};
    mesh->vertices[2] = {glm::vec3(1, 0, 1), glm::vec3(0, 1, 0)};

    // create vao and vbo for rendering
    glGenVertexArrays(1, &mesh->vao);
    if (mesh->vao == 0) {
        fprintf(stderr, "[error] failed to generate vao for mesh\n");
        return false;
    }
    glBindVertexArray(mesh->vao);

    glGenBuffers(1, &mesh->vbo);
    if (mesh->vbo == 0) {
        fprintf(stderr, "[error] failed to generate vbo for mesh\n");
        return false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, normal));

    glBindVertexArray(0);

    return true;
}

void assets_unload() {
    for (u32 i = 0; i < NUM_MESHES; ++i) {
        Mesh *mesh = &meshes[i];
        free(mesh->vertices);
        glDeleteBuffers(1, &mesh->vbo);
        glDeleteVertexArrays(1, &mesh->vao);
    }
}


static bool acorn_init() {
    u32 window_width = 800;
    u32 window_height = 600;
    const char *window_title = "acorn";

    if (!window_init(window_width, window_height, window_title)) return false;
    if (!renderer_init()) return false;
    if (!assets_load()) return false;

    puts("[info] acorn successfully initialized");

    return true;
}

static void acorn_shutdown() {
    puts("[info] shutting down acorn");

    assets_unload();
    renderer_shutdown();
    window_shutdown();
}

static void acorn_run() {
    while (!window_should_close()) {
        window_update();

        // queue test renderable, TODO: remove
        renderer_queue_renderable(Renderable{
                Transform{
                        glm::vec3(-0.5f, 0.5f, -1),
                        glm::quat(glm::vec3(glm::half_pi<f32>(), 0, 0))
                },
                &meshes[0],
                &materials[0]
        });

        // draw frame
        renderer_draw();

        window_swap_buffers();
    }
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
