#ifndef ACORN_SHADER_H
#define ACORN_SHADER_H

#include "types.h"
#include <glm/glm.hpp>

u32 shader_create(const char *vertex_src, const char *fragment_src);

void shader_destroy(u32 shader);

void shader_bind(u32 shader);

void shader_set_vec3(u32 shader, const char *uniform_name, glm::vec3 vec);

void shader_set_mat4(u32 shader, const char *uniform_name, glm::mat4 vec);

#endif //ACORN_SHADER_H
