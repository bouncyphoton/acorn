#ifndef ACORN_UTILS_H
#define ACORN_UTILS_H

#include "types.h"
#include <string>

char *load_file_as_string(const char *filepath);

u32 load_shader_from_file(const char *vertex_file_path, const char *fragment_file_path);

#endif //ACORN_UTILS_H
