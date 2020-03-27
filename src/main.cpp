#include "window.h"
#include "renderer.h"
#include "game_state.h"
#include "model.h"
#include <GLFW/glfw3.h>
#include <cstdio>

#define NUM_MODELS 1
// TODO: move data, preferably to a memory arena
static Model models[NUM_MODELS] = {};

static bool assets_load() {
    // load models
    models[0] = model_load("../assets/sponza/sponza.obj", "../assets/sponza");

    return true;
}

static void assets_unload() {
    for (u32 i = 0; i < NUM_MODELS; ++i) {
        model_free(&models[i]);
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
            game_state.camera.position = glm::vec3(cos(glfwGetTime() * 0.5f) * 5, 7, -4);
            game_state.camera.look_at = game_state.camera.position + glm::vec3(-sin(glfwGetTime() * 0.5f), -1.5, 4);

            // sponza
            renderer_queue_renderable(Renderable{
                    Transform{
                            glm::vec3(0),
                            glm::identity<glm::quat>(),
                            glm::vec3(0.01f)
                    },
                    &models[0]
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
