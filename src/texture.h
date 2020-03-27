#ifndef ACORN_TEXTURE_H
#define ACORN_TEXTURE_H

#include "types.h"

enum class BuiltInTextureEnum {
    MISSING, BLACK, WHITE, NORMAL
};

bool textures_init();

void textures_shutdown();

u32 texture_get(const char *path, BuiltInTextureEnum default_tex = BuiltInTextureEnum::MISSING);

u32 texture_get_built_in(BuiltInTextureEnum texture);

#endif //ACORN_TEXTURE_H
