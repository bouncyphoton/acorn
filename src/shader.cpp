#include "shader.h"
#include "core.h"
#include "utils.h"
#include <GL/gl3w.h>
#include <vector>

Shader::Shader(const std::string &vertex_path, const std::string &fragment_path)
        : m_vertexPath(vertex_path), m_fragmentPath(fragment_path) {
    core->debug("Shader::Shader(" + vertex_path + ", " + fragment_path + ")");
    init();
}

Shader::~Shader() {
    core->debug("Shader::~Shader()");
    destroy();
}

void Shader::reload() {
    destroy();
    init();
}

void Shader::bind() {
    glUseProgram(m_programId);
}

void Shader::setInt(const std::string &name, s32 value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, f32 value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::init() {
    s32 previouslyBound;
    glGetIntegerv(GL_CURRENT_PROGRAM, &previouslyBound);

    u32 vert = 0, frag = 0;
    m_programId = glCreateProgram();
    if (m_programId == 0) {
        core->fatal("Failed to create shader program");
    }

    std::string vertexSrc = utils::load_file_to_string(m_vertexPath.c_str());
    std::string fragmentSrc = utils::load_file_to_string(m_fragmentPath.c_str());

    // Create and compile vertex and fragment shader
    vert = compileAndAttach(GL_VERTEX_SHADER, vertexSrc.c_str(), m_vertexPath.c_str());
    frag = compileAndAttach(GL_FRAGMENT_SHADER, fragmentSrc.c_str(), m_fragmentPath.c_str());

    // Link program
    glLinkProgram(m_programId);
    glDetachShader(m_programId, vert);
    glDetachShader(m_programId, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    // Check link status
    s32 success;
    glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length);
        glGetProgramInfoLog(m_programId, length, &length, log.data());

        core->warn("Failed to link program:\n" + std::string(log.data()));
    }

    glUseProgram(previouslyBound);
}

void Shader::destroy() {
    glDeleteProgram(m_programId);
    m_programId = 0;
    m_uniformLocations.clear();
}

u32 Shader::compileAndAttach(u32 shader_type, const char *shader_src, const char *debug_shader_path) {
    u32 shader = glCreateShader(shader_type);
    if (shader == 0) {
        core->fatal("Failed to create shader \"" + std::string(debug_shader_path) + "\"");
    }
    glShaderSource(shader, 1, &shader_src, nullptr);
    glCompileShader(shader);

    // Check compile status
    s32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length);
        glGetShaderInfoLog(shader, length, &length, log.data());

        core->warn("Failed to compile shader \"" + std::string(debug_shader_path) + "\":\n" + std::string(log.data()));
    }

    // Attach if successfully compiled
    glAttachShader(m_programId, shader);

    return shader;
}

u32 Shader::getUniformLocation(const std::string &name) {
    auto it = m_uniformLocations.find(name);
    if (it == m_uniformLocations.end()) {
        m_uniformLocations[name] = glGetUniformLocation(m_programId, name.c_str());
    }

    return m_uniformLocations[name];
}
