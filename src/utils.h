#ifndef ACORN_UTILS_H
#define ACORN_UTILS_H

#include "types.h"
#include "graphics/vertex.h"
#include <string>

enum class TextureFormatEnum;

namespace utils {
/// Try to load and preprocess a shader from a file
std::string load_shader_to_string(const char *file_path);

/// Get date and time as string
std::string get_date_time_as_string();

/// Generate bi-tangent and tangent vectors for vertices of a triangle
void calculate_tangent_and_bi_tangent(Vertex &v1, Vertex &v2, Vertex &v3);

/// Get OpenGL information for a format
void get_format_info(TextureFormatEnum format, u32 *texture_format, u32 *data_format, u32 *data_type);
}

#endif //ACORN_UTILS_H
