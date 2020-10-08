#ifndef ACORN_MATERIAL_H
#define ACORN_MATERIAL_H

#include "types.h"
#include <glm/glm.hpp>

struct Material {
    u32 albedoTexture = 0;
    u32 normalTexture = 0;

    u32 metallicTexture = 0;
    f32 metallicScale = 1.0f;

    u32 roughnessTexture = 0;
    f32 roughnessScale = 1.0f;
};

#endif //ACORN_MATERIAL_H
