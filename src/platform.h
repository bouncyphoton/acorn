#ifndef ACORN_PLATFORM_H
#define ACORN_PLATFORM_H

#include "types.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

enum class KeyStateEnum {
    UP, DOWN, PRESSED
};

struct Input {
    std::unordered_map<u32, KeyStateEnum> keyStates;
    glm::vec2 cursorPosition;
};

class Platform {
public:
    Platform();
    ~Platform();

    void update();

    GLFWwindow *getGlfwWindow();

    bool isKeyDown(u32 key) const;

    bool isKeyPressed(u32 key) const;

    void setMouseGrab(bool grabbed);

    bool isMouseGrabbed() const;

    glm::vec2 getMouseDelta() const;

    float getDeltaTime() const;

private:
    void init();

    void destroy();

    GLFWwindow *m_window = nullptr;

    Input m_input;
    glm::vec2 m_previousCursorPosition = glm::vec2(0);

    f32 m_previousTime;
    f32 m_currentTime;
    f32 m_deltaTime;
};

#endif //ACORN_PLATFORM_H
