#include "platform.h"
#include "log.h"
#include "constants.h"

static void glfw_error_callback(int error, const char *desc) {
    Log::warn("[error][glfw][%d] %s", error, desc);
}

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto &input = *(Input *)glfwGetWindowUserPointer(window);

    if (action == GLFW_PRESS) {
        input.keyStates[key] = KeyStateEnum::PRESSED;
    } else if (action == GLFW_RELEASE) {
        input.keyStates[key] = KeyStateEnum::UP;
    }
}

static void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y) {
    auto &input = *(Input *)glfwGetWindowUserPointer(window);
    input.cursorPosition = glm::vec2(x, -y);
}

Platform::Platform() {
    Log::debug("Platform::Platform()");
    if (!glfwInit()) {
        Log::fatal("Failed to init GLFW");
    }

    const ConfigData &config = core->config.getConfigData();

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, consts::OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, consts::OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(config.width, config.height, consts::APP_NAME, nullptr, nullptr);

    if (!m_window) {
        Log::fatal("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_window, &m_input);
    glfwSetKeyCallback(m_window, glfw_key_callback);
    glfwSetCursorPosCallback(m_window, glfw_cursor_pos_callback);
    glfwMakeContextCurrent(m_window);

    if (gl3wInit()) {
        Log::fatal("Failed to init gl3w");
    }

    glfwSwapInterval(config.vsyncNumSwapFrames);

    m_currentTime = glfwGetTime();
}

Platform::~Platform() {
    Log::debug("Platform::~Platform()");
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Platform::update() {
    glfwSwapBuffers(m_window);

    // Update key presses from previous update
    for (auto &keyPair : m_input.keyStates) {
        if (keyPair.second == KeyStateEnum::PRESSED) {
            keyPair.second = KeyStateEnum::DOWN;
        }
    }

    m_previousCursorPosition = m_input.cursorPosition;

    // Get latest input
    glfwPollEvents();

    if (glfwWindowShouldClose(m_window)) {
        core->quit();
    }

    m_previousTime = m_currentTime;
    m_currentTime = glfwGetTime();
    m_deltaTime = m_currentTime - m_previousTime;
}

GLFWwindow *Platform::getGlfwWindow() {
    return m_window;
}

bool Platform::isKeyDown(u32 key) const {
    auto it = m_input.keyStates.find(key);
    if (it != m_input.keyStates.end()) {
        return it->second != KeyStateEnum::UP;
    }
    return false;
}

bool Platform::isKeyPressed(u32 key) const {
    auto it = m_input.keyStates.find(key);
    if (it != m_input.keyStates.end()) {
        return it->second == KeyStateEnum::PRESSED;
    }
    return false;
}

void Platform::setMouseGrab(bool grabbed) {
    if (grabbed) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_previousCursorPosition = glm::vec2(0);
        m_input.cursorPosition = glm::vec2(0);
    }
}

bool Platform::isMouseGrabbed() const {
    return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

glm::vec2 Platform::getMouseDelta() const {
    return m_input.cursorPosition - m_previousCursorPosition;
}

float Platform::getDeltaTime() const {
    return m_deltaTime;
}
