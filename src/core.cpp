#include "core.h"
#include <iostream>

static Core core_local;
Core *core = &core_local;

void Core::run() {
    info("Initializing...");

    init();

    info("Initialized successfully");

    Entity sphereEntity = {
            resourceManager.getModel("../assets/spheres/spheres.obj"),
            Transform{
                    glm::vec3(0, 0.15, 0),
                    glm::identity<glm::quat>(),
                    glm::vec3(1.0f)
            }};

    Entity rifleEntity = {
            resourceManager.getModel("../assets/stylized-rifle/Stylized_rifle_final.obj"),
            Transform{
                    glm::vec3(0, -0.45, -0.35),
                    glm::vec3(0, glm::half_pi<f32>(), 0),
                    glm::vec3(0.01f)
            }};

    Entity rockEntity = {
            resourceManager.getModel("../assets/rock03/3DRock003_16K.obj"),
            Transform{
                    glm::vec3(0, -0.45, 0.35),
                    glm::vec3(0, glm::half_pi<f32>(), 0),
                    glm::vec3(1.0f)
            }};

    entityHandle_t sphereHandle = gameState.scene.addEntity(sphereEntity);
    entityHandle_t rifleHandle = gameState.scene.addEntity(rifleEntity);
    entityHandle_t rockHandle = gameState.scene.addEntity(rockEntity);

    while (true) {
        platform.update();

        // TODO: remove temporary update
        {
            if (gameState.camera.isOrbiting) {
                f32 t = glfwGetTime() * 0.25f;
                gameState.camera.position = glm::vec3(cos(t) * 2, 0, sin(t) * 2);
                gameState.camera.lookAt = glm::vec3(0, 0, 0);
            }

            // Make spheres float up and down
            sphereEntity.transform.position.y = 0.15 + sinf(glfwGetTime()) * 0.05;
            gameState.scene.updateEntity(sphereHandle, sphereEntity);
        }

        // draw frame
        renderer.render();

        debugGui.draw();
    }
}

void Core::quit() {
    info("Quitting normally");
    cleanup();
    exit(0);
}

void Core::info(const std::string &msg) {
    std::cout << "[info] " << msg << std::endl;
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
    gameState.renderOptions.width = 1024;
    gameState.renderOptions.height = 768;
    gameState.camera.position = glm::vec3(1, 0, 0);
    gameState.camera.lookAt = glm::vec3(0, 0, -1);
    gameState.camera.fovRadians = glm::quarter_pi<f32>();
    gameState.sunDirection = glm::normalize(glm::vec3(-1, 1, 1));

    platform.init();
    renderer.init();
    resourceManager.init();
    debugGui.init();
}

void Core::cleanup() {
    debugGui.destroy();
    resourceManager.destroy();
    renderer.destroy();
    platform.destroy();
}
