#ifndef ACORN_PLATFORM_H
#define ACORN_PLATFORM_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Platform {
public:
    void init();

    void destroy();

    void update();

    GLFWwindow *getGlfwWindow();

private:
    GLFWwindow *m_window;
};

#endif //ACORN_PLATFORM_H
