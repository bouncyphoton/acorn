#include "platform.h"
#include "core.h"
#include "constants.h"

static void glfw_error_callback(int error, const char *desc) {
    fprintf(stderr, "[error][glfw][%d] %s\n", error, desc);
}

void Platform::init() {
    if (!glfwInit()) {
        core->fatal("Failed to init GLFW");
    }

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, consts::OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, consts::OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(core->gameState.renderOptions.width, core->gameState.renderOptions.height,
                                consts::APP_NAME, nullptr, nullptr);

    if (!m_window) {
        core->fatal("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    if (gl3wInit()) {
        core->fatal("Failed to init gl3w");
    }

    glfwSwapInterval(core->gameState.renderOptions.vsyncNumSwapFrames);
}

void Platform::destroy() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Platform::update() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();

    if (glfwWindowShouldClose(m_window)) {
        core->quit();
    }
}

GLFWwindow *Platform::getGlfwWindow() {
    return m_window;
}
