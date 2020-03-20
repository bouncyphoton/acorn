#ifndef ACORN_SHADER_H
#define ACORN_SHADER_H

#include "types.h"

u32 shader_create(const char *vertex_src, const char *fragment_src);

void shader_destroy(u32 shader);

void shader_bind(u32 shader);

#endif //ACORN_SHADER_H
