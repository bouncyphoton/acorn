#ifndef ACORN_MATERIAL_H
#define ACORN_MATERIAL_H

#include "types.h"
#include <glm/glm.hpp>

struct Material {
    u32 albedo_texture = 0;
    u32 normal_texture = 0;

    u32 metallic_texture = 0;
    f32 metallic_scale = 1.0f;

    u32 roughness_texture = 0;
    f32 roughness_scale = 1.0f;
};

#endif //ACORN_MATERIAL_H
