#ifndef ACORN_WINDOW_H
#define ACORN_WINDOW_H

#include "types.h"
#include "game_state.h"

bool window_init(RenderOptions render_options, const char *title);

void window_shutdown();

void window_update();

void window_swap_buffers();

bool window_should_close();

#endif //ACORN_WINDOW_H
