#ifndef ACORN_GAME_STATE_H
#define ACORN_GAME_STATE_H

#include "types.h"

struct RenderOptions {
    u32 width = 800;
    u32 height = 600;
};

struct Camera {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 look_at = glm::vec3(0, 0, -1);
};

struct GameState {
    RenderOptions render_options = {};
    Camera camera = {};
};

#endif //ACORN_GAME_STATE_H
