#ifndef ACORN_PLATFORM_H
#define ACORN_PLATFORM_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Platform {
public:
    Platform();
    ~Platform();

    void update();

    GLFWwindow *getGlfwWindow();

private:
    void init();

    void destroy();

    GLFWwindow *m_window;
};

#endif //ACORN_PLATFORM_H
