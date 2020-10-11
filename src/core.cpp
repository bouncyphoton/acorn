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
    // TODO: ECS
    Entity helmetEntity = {
        resourceManager.getModel("../assets/glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf"),
        Transform{
            glm::vec3(0, 0, 0),
            glm::identity<glm::quat>(),
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

    entityHandle_t helmetHandle = gameState.scene.addEntity(helmetEntity);
    entityHandle_t planeHandle = gameState.scene.addEntity(planeEntity);

    gameState.camera.setPosition(glm::vec3(1.5, 1, -2));
    gameState.camera.setLookRotation(glm::vec2(glm::half_pi<f32>(), 0));

    while (true) {
        platform.update();

        f32 speed = platform.isKeyDown(GLFW_KEY_LEFT_SHIFT) ? 10.0f : 1.0f;
        f32 dt = platform.getDeltaTime();
        if (platform.isKeyDown(GLFW_KEY_W)) {
            gameState.camera.addPosition(gameState.camera.getForward() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_S)) {
            gameState.camera.addPosition(-gameState.camera.getForward() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_D)) {
            gameState.camera.addPosition(gameState.camera.getRight() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_A)) {
            gameState.camera.addPosition(-gameState.camera.getRight() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_SPACE)) {
            gameState.camera.addPosition(Camera::UP * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
            gameState.camera.addPosition(-Camera::UP * speed * dt);
        }

        if (platform.isKeyPressed(GLFW_KEY_TAB)) {
            platform.setMouseGrab(!platform.isMouseGrabbed());
        }

        if (platform.isMouseGrabbed()) {
            glm::vec2 mouseDelta = platform.getMouseDelta() / glm::vec2(gameState.renderOptions.width,
                                                                        gameState.renderOptions.height);
            gameState.camera.addLookRotation(mouseDelta);
        }

        renderer.render();

        debugGui.draw();
    }
}

void Core::quit() {
    Log::info("Quitting normally");
    exit(0);
}
