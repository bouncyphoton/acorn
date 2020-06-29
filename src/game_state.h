#ifndef ACORN_GAME_STATE_H
#define ACORN_GAME_STATE_H

#include "types.h"
#include "scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct RenderOptions {
    u32 width = 800;
    u32 height = 600;
    u32 vsyncNumSwapFrames = 0;
};

struct Camera {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 lookAt = glm::vec3(0, 0, -1);
    f32 fovRadians = glm::half_pi<f32>();
    bool isOrbiting = true;
};

struct GameState {
    RenderOptions renderOptions = {};
    Camera camera = {};
    Scene scene = {};

    // unit vector pointing towards the sun
    glm::vec3 sunDirection = glm::vec3(0, 1, 0);
};

#endif //ACORN_GAME_STATE_H
