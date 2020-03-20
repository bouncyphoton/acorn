#include "renderer.h"
#include "types.h"
#include "shader.h"
#include <GL/gl3w.h>
#include <cstdio>

static u32 fullscreen_shader = 0;

const char *fullscreen_vertex_src = R"(#version 430 core
out VertexData {
    vec2 uv;
} o;

void main() {
    float x = float(((uint(gl_VertexID) + 2u) / 3u) % 2u);
    float y = float(((uint(gl_VertexID) + 1u) / 3u) % 2u);

    gl_Position = vec4(x * 2.0f - 1.0f, y * 2.0f - 1.0f, 0.0f, 1.0f);
    o.uv = vec2(x, y);
})";

const char *fullscreen_fragment_src = R"(#version 430 core
layout (location = 0) out vec4 oFragColor;

in VertexData {
    vec2 uv;
} i;

void main() {
    oFragColor = vec4(i.uv, vec2(0.5, 1));
})";

static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar *message, const void *user_param) {
    fprintf(severity == GL_DEBUG_SEVERITY_HIGH ? stderr : stdout, "[debug][opengl] %s\n", message);
}

bool renderer_init() {
    // NOTE: debug output is not a part of core opengl until 4.3, but it's ok
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);

    // init shaders
    fullscreen_shader = shader_create(fullscreen_vertex_src, fullscreen_fragment_src);
    if (fullscreen_shader == 0) {
        return false;
    }

    return true;
}

void renderer_shutdown() {
    shader_destroy(fullscreen_shader);
}

void renderer_draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: remove test drawing

    static u32 vao = 0;
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
    }

    // bind fullscreen shader
    shader_bind(fullscreen_shader);

    // send 6 vertices to bound shader
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
