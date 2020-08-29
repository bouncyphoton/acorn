#include "utils.h"
#include "core.h"
#include "texture.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace utils {
    std::string load_file_to_string(const char *file_path) {
        if (std::ifstream ifs = std::ifstream(file_path, std::ios::in)) {
            std::string contents;

            ifs.seekg(0, std::ios::end);
            contents.resize(ifs.tellg());
            ifs.seekg(0, std::ios::beg);

            ifs.read(&contents[0], contents.size());
            return contents;
        }

        core->fatal("Failed to load file to string \"" + std::string(file_path) + "\"");
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
            default:
                core->fatal("Tried to get info for unknown format: " + std::to_string(static_cast<u32>(format)));
        }
    }
}
