#include "window.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cstdio>

GLFWwindow *window = nullptr;

static void glfw_error_callback(int error, const char *desc) {
    fprintf(stderr, "[error][glfw][%d] %s\n", error, desc);
}

bool window_init(RenderOptions render_options, const char *title) {
    if (!glfwInit()) {
        fprintf(stderr, "[error] failed to init GLFW\n");
        return false;
    }

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(render_options.width, render_options.height, title, nullptr, nullptr);

    if (!window) {
        fprintf(stderr, "[error] failed to create window\n");
        return false;
    }

    glfwMakeContextCurrent(window);

    if (gl3wInit()) {
        fprintf(stderr, "[error] failed to init gl3w\n");
        return false;
    }

    glfwSwapInterval(render_options.vsync_num_swap_frames);

    return true;
}

void window_shutdown() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void window_poll_events() {
    glfwPollEvents();
}

void window_swap_buffers() {
    glfwSwapBuffers(window);
}

bool window_should_close() {
    return glfwWindowShouldClose(window);
}
