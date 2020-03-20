#include "shader.h"
#include <GL/gl3w.h>
#include <vector>

// TODO: make prettier
u32 shader_create(const char *vertex_src, const char *fragment_src) {
    u32 shader = glCreateProgram();
    if (shader == 0) return shader;

    // compile and attach vertex shader
    u32 vert_id = glCreateShader(GL_VERTEX_SHADER);
    if (vert_id == 0) {
        shader_destroy(shader);
        return 0;
    }
    glShaderSource(vert_id, 1, &vertex_src, nullptr);
    glCompileShader(vert_id);
    s32 success = 0;
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(vert_id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(vert_id, length, &length, log.data());
        fprintf(stderr, "[error][opengl] vertex shader failed to compile:\n %s\n", log.data());

        shader_destroy(shader);
        return 0;
    }
    glAttachShader(shader, vert_id);

    // compile and attach fragment shader
    u32 frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    if (frag_id == 0) {
        shader_destroy(shader);
        return 0;
    }
    glShaderSource(frag_id, 1, &fragment_src, nullptr);
    glCompileShader(frag_id);
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(frag_id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(frag_id, length, &length, log.data());
        fprintf(stderr, "[error][opengl] fragment shader failed to compile:\n %s\n", log.data());

        glDetachShader(shader, vert_id);
        glDeleteShader(vert_id);
        shader_destroy(shader);
        return 0;
    }
    glAttachShader(shader, frag_id);

    // link shader stages to shader program
    glLinkProgram(shader);
    glDetachShader(shader, vert_id);
    glDetachShader(shader, frag_id);
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);

    success = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(shader, length, &length, log.data());
        fprintf(stderr, "[error][opengl] shader failed to link %s\n", log.data());

        shader_destroy(shader);
        return 0;
    }

    return shader;
}

void shader_destroy(u32 shader) {
    glDeleteProgram(shader);
}

void shader_bind(u32 shader) {
    glUseProgram(shader);
}
