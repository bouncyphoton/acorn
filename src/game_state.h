#ifndef ACORN_GAME_STATE_H
#define ACORN_GAME_STATE_H

#include "types.h"
#include "scene.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct RenderOptions {
    u32 width = 800;
    u32 height = 600;
    u32 vsyncNumSwapFrames = 1;
};

struct GameState {
    RenderOptions renderOptions = {};
    Scene scene = {};
    Camera camera;
};

#endif //ACORN_GAME_STATE_H
