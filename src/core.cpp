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

    // Wall
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            scene.addGeometry(
                Transform(
                    glm::vec3(4 * j, 0, -5 + 10 * i),
                    glm::vec3(0, glm::pi<f32>() * i, 0),
                    glm::vec3(1.0f)
                ), resourceManager.getModel("../assets/KayKit/models/wallSingle.gltf.glb"));
        }
    }

    // Boombox
    scene.addGeometry(
            Transform(
                    glm::vec3(0, 2, 0),
                    glm::vec3(0, 0, 0),
                    glm::vec3(100.0f)
            ), resourceManager.getModel("../assets/glTF-Sample-Models/2.0/BoomBox/glTF/BoomBox.gltf")
    );

    // IBL probe
    scene.addIblProbe(glm::vec3(0, 3, 0));

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
            glm::vec2 mouseDelta = platform.getMouseDelta() / glm::vec2(config.getConfigData().width,
                                                                        config.getConfigData().height);
            camera.addLookRotation(mouseDelta);
        }

        renderer.update(scene);
        renderer.render(scene);

        debugGui.draw();
    }
}

void Core::quit() {
    Log::info("Quitting normally");
    exit(0);
}
