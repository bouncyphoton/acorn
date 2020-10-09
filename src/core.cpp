#include "core.h"
#include "utils.h"
#include <iostream>

static Core core_local;
Core *core = &core_local;

Core::Core() {
    debug("Core::Core()");
}

Core::~Core() {
    debug("Core::~Core()");
}

void Core::run() {
    // TODO: make this less weird to work with
    Entity sphereEntity = {
        resourceManager.getModel("../assets/spheres/spheres.obj"),
        Transform{
            glm::vec3(0, 1.15, 0),
            glm::identity<glm::quat>(),
            glm::vec3(1.0f)
        }
    };

    Entity rifleEntity = {
        resourceManager.getModel("../assets/stylized-rifle/Stylized_rifle_final.obj"),
        Transform{
            glm::vec3(0, 0.55, -0.35),
            glm::vec3(0, glm::half_pi<f32>(), 0),
            glm::vec3(0.01f)
        }
    };

    Entity rockEntity = {
        resourceManager.getModel("../assets/rock03/3DRock003_16K.obj"),
        Transform{
            glm::vec3(0, 0.55, 0.35),
            glm::vec3(0, glm::half_pi<f32>(), 0),
            glm::vec3(1.0f)
        }
    };

    Entity planeEntity = {
        resourceManager.getBuiltInModel(BuiltInModelEnum::PLANE),
        Transform{
            glm::vec3(0),
            glm::identity<glm::quat>(),
            glm::vec3(10)
        }
    };

    entityHandle_t sphereHandle = gameState.scene.addEntity(sphereEntity);
    entityHandle_t rifleHandle = gameState.scene.addEntity(rifleEntity);
    entityHandle_t rockHandle = gameState.scene.addEntity(rockEntity);
    entityHandle_t planeHandle = gameState.scene.addEntity(planeEntity);

    while (true) {
        platform.update();

        // TODO: remove temporary update
        {
            if (gameState.camera.isOrbiting) {
                f32 t = glfwGetTime() * 0.25f;
                gameState.camera.position = glm::vec3(cos(t) * 2, 1, sin(t) * 2);
                gameState.camera.lookAt = glm::vec3(0, 1, 0);
            }
        }

        // draw frame
        renderer.render();

        debugGui.draw();
    }
}

void Core::quit() {
    info("Quitting normally");
    exit(0);
}

static void generic_log(const std::string &msg) {
    std::cout << "[" << utils::get_date_time_as_string() << "]" << msg << std::endl;
}

void Core::info(const std::string &msg) {
    generic_log("[info] " + msg);
}

void Core::warn(const std::string &msg) {
    generic_log("[warn] " + msg);
}

void Core::debug(const std::string &msg) {
    generic_log("[debug] " + msg);
}

void Core::fatal(const std::string &msg) {
    generic_log("[fatal] " + msg);
    exit(1);
}
