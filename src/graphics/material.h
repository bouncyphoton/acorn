#ifndef ACORN_MATERIAL_H
#define ACORN_MATERIAL_H

#include "types.h"
#include "texture.h"
#include <glm/glm.hpp>

// TODO: if we decide to stream textures or something, we will want a better handle for textures

struct Material {
    Texture *albedoTexture = nullptr;
    Texture *normalTexture = nullptr;

    Texture *metallicTexture = nullptr;
    f32 metallicScale = 1.0f;

    Texture *roughnessTexture = nullptr;
    f32 roughnessScale = 1.0f;
};

#endif //ACORN_MATERIAL_H
