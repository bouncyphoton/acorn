#include "core.h"
#include "log.h"
#include <iostream>

static Core core_local;
Core *core = &core_local;

Core::Core() {
    Log::debug("Core::Core()");
}

Core::~Core() {
    Log::debug("Core::~Core()");
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

    gameState.camera.setPosition(glm::vec3(1.5, 1, -2));
    gameState.camera.setLookPosition(glm::vec3(0.75, 1, 0));

    while (true) {
        platform.update();

        renderer.render();

        debugGui.draw();
    }
}

void Core::quit() {
    Log::info("Quitting normally");
    exit(0);
}
