#include "core.h"
#include "model.h"
#include "texture.h"
#include <iostream>

static Core core_local;
Core *core = &core_local;

void Core::run() {
    info("Initializing...");

    init();

    info("Initialized successfully");

    while (true) {
        m_platform.update();

        // TODO: remove temporary update
        {
            if (game_state.camera.is_orbiting) {
                f32 t = glfwGetTime() * 0.25f;
                game_state.camera.position = glm::vec3(cos(t) * 2, 0, sin(t) * 2);
                game_state.camera.look_at = glm::vec3(0, 0, 0);
            }

            Model *spheres = resourceManager.getModel("../assets/spheres/spheres.obj");
            Model *rifle = resourceManager.getModel("../assets/stylized-rifle/Stylized_rifle_final.obj");
            Model *rock = resourceManager.getModel("../assets/rock03/3DRock003_16K.obj");

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, 0.15, 0),
                            glm::identity<glm::quat>(),
                            glm::vec3(1.0f)
                    },
                    spheres
            });

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, -0.45, -0.35),
                            glm::vec3(0, glm::half_pi<f32>(), 0),
                            glm::vec3(0.01f)
                    },
                    rifle
            });

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, -0.45, 0.35),
                            glm::vec3(0, glm::half_pi<f32>(), 0),
                            glm::vec3(1.0f)
                    },
                    rock
            });

            m_renderer.queueRenderable(Renderable{
                    Transform{
                            glm::vec3(0, -0.45, 0.35),
                            glm::vec3(0, glm::half_pi<f32>(), 0),
                            glm::vec3(1.0f)
                    },
                    rock
            });
        }

        // draw frame
        m_renderer.render();

//        debug_gui_draw(&game_state);
    }
}

void Core::quit() {
    info("Quitting normally");
    cleanup();
    exit(0);
}

void Core::info(const std::string &msg) {
    std::cout << "[msg] " << msg << std::endl;
}

void Core::warn(const std::string &msg) {
    std::cerr << "[warn] " << msg << std::endl;
}

void Core::fatal(const std::string &msg) {
    std::cerr << "[fatal] " << msg << std::endl;
    cleanup();
    exit(1);
}

void Core::init() {
    game_state.render_options.width = 1024;
    game_state.render_options.height = 768;
    game_state.camera.position = glm::vec3(1, 0, 0);
    game_state.camera.look_at = glm::vec3(0, 0, -1);
    game_state.camera.fov_radians = glm::quarter_pi<f32>();
    game_state.sun_direction = glm::normalize(glm::vec3(-1, 1, 1));

    m_platform.init();
    m_renderer.init();
    resourceManager.init();
    if (!textures_init()) core->fatal("Failed to init textures");
//    if (!debug_gui_init()) return false;

//    game_state.scene.addModel("../assets/spheres/spheres.obj");
//    game_state.scene.addModel("../assets/stylized-rifle/Stylized_rifle_final.obj");
//    game_state.scene.addModel("../assets/rock03/3DRock003_16K.obj");
}

void Core::cleanup() {
//    debug_gui_shutdown();
    textures_shutdown();
    resourceManager.destroy();
    m_renderer.destroy();
    m_platform.destroy();
}
