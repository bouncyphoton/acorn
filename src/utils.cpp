#include "utils.h"
#include "core.h"
#include "log.h"
#include "graphics/texture.h"

#define STB_INCLUDE_IMPLEMENTATION
#define STB_INCLUDE_LINE_GLSL

#include <stb_include.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace utils {
std::string load_shader_to_string(const char *file_path) {
    // find last slash in filepath for directory
    u32 lastSlash = 0;
    const char *current = file_path;
    for (u32 i = 0; *current; ++i, ++current) {
        if (*current == '/') {
            lastSlash = i;
        }
    }

    // get directory
    std::string directory = std::string(file_path).substr(0, lastSlash);

    char error[256];
    const char *inject = "";
    char *str = stb_include_file(const_cast<char *>(file_path), const_cast<char *>(inject),
                                 const_cast<char *>(directory.c_str()), error);
    if (!str) {
        Log::warn("Failed to load/preprocess shader: %s", error);
        return "";
    }

    return str;
}

std::string get_date_time_as_string() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream os;
    os << std::put_time(&tm, "%F %T");
    return os.str();
}

void calculate_tangent_and_bi_tangent(Vertex &v1, Vertex &v2, Vertex &v3) {
    glm::vec3 deltaPos1 = v2.position - v1.position;
    glm::vec3 deltaPos2 = v3.position - v1.position;

    glm::vec2 deltaUv1 = v2.uv - v1.uv;
    glm::vec2 deltaUv2 = v3.uv - v1.uv;

    float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUv2.y - deltaPos2 * deltaUv1.y) * r;
    glm::vec3 biTangent = (deltaPos2 * deltaUv1.x - deltaPos1 * deltaUv2.x) * r;

    // make tangent orthogonal to normal
    v1.tangent = glm::normalize(tangent - v1.normal * glm::dot(v1.normal, tangent));
    v1.biTangent = biTangent;

    v2.tangent = glm::normalize(tangent - v2.normal * glm::dot(v2.normal, tangent));
    v2.biTangent = biTangent;

    v3.tangent = glm::normalize(tangent - v3.normal * glm::dot(v3.normal, tangent));
    v3.biTangent = biTangent;
}

void get_format_info(TextureFormatEnum format, u32 *texture_format, u32 *data_format, u32 *data_type) {
    *texture_format = 0;
    *data_format = 0;
    *data_type = 0;

    switch (format) {
        case TextureFormatEnum::R8:
            *texture_format = GL_RED;
            *data_format = GL_RED;
            *data_type = GL_UNSIGNED_BYTE;
            break;
        case TextureFormatEnum::RGB8:
            *texture_format = GL_RGB;
            *data_format = GL_RGB;
            *data_type = GL_UNSIGNED_BYTE;
            break;
        case TextureFormatEnum::RGBA8:
            *texture_format = GL_RGBA;
            *data_format = GL_RGBA;
            *data_type = GL_UNSIGNED_BYTE;
            break;
        case TextureFormatEnum::RG16F:
            *texture_format = GL_RG16F;
            *data_format = GL_RG;
            *data_type = GL_FLOAT;
            break;
        case TextureFormatEnum::RGB16F:
            *texture_format = GL_RGB16F;
            *data_format = GL_RGB;
            *data_type = GL_FLOAT;
            break;
        case TextureFormatEnum::RGBA16F:
            *texture_format = GL_RGBA16F;
            *data_format = GL_RGBA;
            *data_type = GL_FLOAT;
            break;
        case TextureFormatEnum::RGB32F:
            *texture_format = GL_RGB32F;
            *data_format = GL_RGB;
            *data_type = GL_FLOAT;
            break;
        case TextureFormatEnum::RGBA32F:
            *texture_format = GL_RGBA32F;
            *data_format = GL_RGBA;
            *data_type = GL_FLOAT;
            break;
        default:
            Log::fatal("Tried to get info for unknown format: %d", (u32)format);
    }
}
}
