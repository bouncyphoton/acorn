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

    Scene &scene = gameState.scene;
    Camera &camera = scene.getCamera();

    camera.setPosition(glm::vec3(1.5, 1, -2));
    camera.setLookRotation(glm::vec2(glm::half_pi<f32>(), 0));

    // Boombox
    scene.addGeometry(
        Transform(
            glm::vec3(0, 2, 0),
            glm::vec3(0, glm::half_pi<f32>(), 0),
            glm::vec3(100.0f)
        ), resourceManager.getModel("../assets/glTF-Sample-Models/2.0/BoomBox/glTF/BoomBox.gltf"));

    // Helmet
    scene.addGeometry(
        Transform(
            glm::vec3(0, 0, 2.5),
            glm::vec3(0, glm::three_over_two_pi<f32>(), 0),
            glm::vec3(5.0f)
        ), resourceManager.getModel("../assets/glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf")
    );

    while (true) {
        platform.update();

        f32 speed = platform.isKeyDown(GLFW_KEY_LEFT_SHIFT) ? 10.0f : 1.0f;
        f32 dt = platform.getDeltaTime();
        if (platform.isKeyDown(GLFW_KEY_W)) {
            camera.addPosition(camera.getForward() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_S)) {
            camera.addPosition(-camera.getForward() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_D)) {
            camera.addPosition(camera.getRight() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_A)) {
            camera.addPosition(-camera.getRight() * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_SPACE)) {
            camera.addPosition(Camera::UP * speed * dt);
        }
        if (platform.isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
            camera.addPosition(-Camera::UP * speed * dt);
        }

        if (platform.isKeyPressed(GLFW_KEY_TAB)) {
            platform.setMouseGrab(!platform.isMouseGrabbed());
        }

        if (platform.isMouseGrabbed()) {
            glm::vec2 mouseDelta = platform.getMouseDelta() / glm::vec2(gameState.renderOptions.width,
                                                                        gameState.renderOptions.height);
            camera.addLookRotation(mouseDelta);
        }

        renderer.render(gameState.scene);

        debugGui.draw();
    }
}

void Core::quit() {
    Log::info("Quitting normally");
    exit(0);
}
