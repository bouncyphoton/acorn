#include "window.h"
#include "renderer.h"
#include "texture.h"
#include "model.h"
#include "game_state.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <chrono>

#define NUM_MODELS 1
// TODO: move data, preferably to a memory arena
static Model models[NUM_MODELS] = {};

// TODO: stop hardcoding assets directory

static bool assets_load() {
    // load models
    models[0] = model_load("../assets/stylized-rifle/Stylized_rifle_final.obj", "../assets/stylized-rifle");

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
    game_state.sun_direction = glm::normalize(glm::vec3(-1, 1, 1));

    if (!window_init(game_state.render_options, window_title)) return false;
    if (!renderer_init(game_state.render_options)) return false;
    if (!textures_init()) return false;
    if (!assets_load()) return false;

    puts("[info] acorn successfully initialized");

    return true;
}

static void acorn_shutdown() {
    puts("[info] shutting down acorn");

    assets_unload();
    textures_shutdown();
    renderer_shutdown();
    window_shutdown();
}

static void acorn_run() {
    auto last = std::chrono::system_clock::now();
    while (!window_should_close()) {
        auto now = std::chrono::system_clock::now();
        f32 ms_since_last_frame = std::chrono::duration_cast<std::chrono::microseconds>(now - last).count() / 1000.0f;
        RenderStats stats = renderer_get_stats();
        printf("frame took %.2fms (%d fps)\n", ms_since_last_frame, (int) (1000.0f / ms_since_last_frame));
        printf("render stats:\nverts: %d\ncalls: %d\n\n", stats.vertices_rendered, stats.draw_calls);
        last = now;

        window_update();

        // TODO: remove temporary update
        {
            f32 t = glfwGetTime() * 0.5f;
            game_state.camera.position = glm::vec3(cos(t) * 2, 0.5, sin(t) * 2);
            game_state.camera.look_at = glm::vec3(0, 0, 0);

            // stylized gun
            Model *gun_model = &models[0];

            renderer_queue_renderable(Renderable{
                    Transform{
                            glm::vec3(0, 0, -0.5f),
                            glm::identity<glm::quat>(),
                            glm::vec3(0.05f)
                    },
                    gun_model
            });

            renderer_queue_renderable(Renderable{
                    Transform{
                            glm::vec3(0, 0, 0.5f),
                            glm::quat(glm::vec3(0, glm::pi<f32>(), 0)),
                            glm::vec3(0.05f)
                    },
                    gun_model
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
