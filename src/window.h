#ifndef ACORN_WINDOW_H
#define ACORN_WINDOW_H

#include "types.h"

bool window_init(u32 width, u32 height, const char *title);

void window_shutdown();

void window_update();

void window_swap_buffers();

bool window_should_close();

#endif //ACORN_WINDOW_H
