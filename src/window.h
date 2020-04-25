#ifndef ACORN_WINDOW_H
#define ACORN_WINDOW_H

#include "types.h"
#include "game_state.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

bool window_init(RenderOptions render_options, const char *title);

void window_shutdown();

void window_poll_events();

void window_swap_buffers();

bool window_should_close();

GLFWwindow *window_get_glfw_window();

#endif //ACORN_WINDOW_H
