#include "window.h"
#include "renderer.h"
#include "game_state.h"
#include <GLFW/glfw3.h>
#include <cstdio>

#define NUM_MESHES 2
#define NUM_MATERIALS 1
// TODO: move meshes and materials data, preferably to a memory arena
static Mesh meshes[NUM_MESHES] = {};
static Material materials[NUM_MATERIALS] = {};

static bool assets_load() {
    // load mesh
    meshes[0] = mesh_load("../assets/naturePack_130.obj", "../assets/");
    meshes[1] = mesh_load("../assets/naturePack_001.obj", "../assets/");

    // load material
    materials[0].color = glm::vec3(1);

    // check that assets loaded correctly
    for (u32 i = 0; i < NUM_MESHES; ++i) {
        Mesh *mesh = &meshes[i];
        if (mesh->vertices == nullptr || mesh->vao == 0 || mesh->vbo == 0) {
            return false;
        }
    }

    return true;
}

static void assets_unload() {
    for (u32 i = 0; i < NUM_MESHES; ++i) {
        mesh_free(&meshes[i]);
    }
}


static GameState game_state = {};

static bool acorn_init() {
    const char *window_title = "acorn";
    game_state.render_options.width = 800;
    game_state.render_options.height = 600;
    game_state.camera.position = glm::vec3(1, 0, 0);
    game_state.camera.look_at = glm::vec3(0, 0, -1);
    game_state.camera.fov_radians = glm::half_pi<f32>();
    game_state.sun_direction = glm::normalize(glm::vec3(1, 1, 1));

    if (!window_init(game_state.render_options.width, game_state.render_options.height, window_title)) return false;
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

        // TODO: remove temporary update
        {
            game_state.camera.look_at = (meshes[0].min + meshes[0].max) / 2.0f;
            game_state.camera.position =
                    glm::vec3(cos(glfwGetTime()), 0, sin(glfwGetTime())) * 5.0f
                    + game_state.camera.look_at;

            // tree
            renderer_queue_renderable(Renderable{
                    Transform{},
                    &meshes[0],
                    &materials[0]
            });

            // ground
            renderer_queue_renderable(Renderable{
                    Transform{
                            glm::vec3(0, -meshes[1].max.y, 0)
                    },
                    &meshes[1],
                    &materials[0]
            });
        }

        // draw frame
        renderer_draw(&game_state);

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
