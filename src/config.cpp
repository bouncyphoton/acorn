#include "config.h"
#include "core.h"

Config::Config() {
    // TODO: load this from a file
    core->gameState.renderOptions.width = 1024;
    core->gameState.renderOptions.height = 768;
    core->gameState.camera.position = glm::vec3(1, 0, 0);
    core->gameState.camera.lookAt = glm::vec3(0, 0, -1);
    core->gameState.camera.fovRadians = glm::quarter_pi<f32>();
    core->gameState.scene.sunDirection = glm::normalize(glm::vec3(-1, 1, 1));
}
