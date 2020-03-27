#ifndef ACORN_RENDERER_H
#define ACORN_RENDERER_H

#include "renderable.h"
#include "game_state.h"

bool renderer_init();

void renderer_shutdown();

void renderer_queue_renderable(Renderable renderable);

void renderer_draw(GameState *game_state);

#endif //ACORN_RENDERER_H
