#ifndef ACORN_SHADER_H
#define ACORN_SHADER_H

#include "types.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

/// OpenGL shader
class Shader {
public:
    Shader(const std::string &vertex_path, const std::string &fragment_path);
    ~Shader();

    /// Bind shader for usage
    void bind();

    /// Set int shader uniform
    void setInt(const std::string &name, s32 value);

    /// Set float shader uniform
    void setFloat(const std::string &name, f32 value);

    /// Set vec3 shader uniform
    void setVec3(const std::string &name, glm::vec3 value);

    /// Set mat4 shader uniform
    void setMat4(const std::string &name, glm::mat4 value);

private:
    /// Load shaders from files
    void init(const std::string &vertex_path, const std::string &fragment_path);

    /// Cleanup shaders
    void destroy();

    u32 compileAndAttach(u32 shader_type, const char *shader_src, const char *debug_shader_path);

    u32 getUniformLocation(const std::string &name);

    u32 m_programId = 0;
    std::unordered_map<std::string, u32> m_uniformLocations;
};

#endif //ACORN_SHADER_H
