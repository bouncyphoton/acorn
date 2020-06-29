#ifndef ACORN_TEXTURE_H
#define ACORN_TEXTURE_H

#include "types.h"
#include <GL/gl3w.h>
#include <string>

enum class BuiltInTextureEnum {
    MISSING, BLACK, WHITE, NORMAL
};

class Texture {
public:
    /// Init a texture from a file
    void init(const std::string &path);

    /// Init a 2D texture with data
    void init(GLenum texture_format, s32 width, s32 height, GLenum data_type, u8 *data, GLenum data_format);

    /// Clean up
    void destroy();

    u32 id;
};

// TODO: finish restyling texture stuff

u32 texture_2d_create(GLenum texture_format, s32 width, s32 height, GLenum data_type, u8 *data, GLenum data_format);

u32 texture_2d_create(GLenum texture_format, u32 width, u32 height, GLenum data_type);

void texture_2d_get_dimensions(u32 texture, u32 miplevel, s32 *width, s32 *height);

u32 texture_cubemap_create(GLenum texture_format, s32 width, s32 height, GLenum data_type, void **data, GLenum data_format);

u32 texture_cubemap_create(GLenum texture_format, s32 width, s32 height, GLenum data_type);

void texture_destroy(u32 texture);

//------------------------
// texture manager-y stuff
//------------------------

bool textures_init();

void textures_shutdown();

u32 texture_get(const char *path, BuiltInTextureEnum default_tex = BuiltInTextureEnum::MISSING);

u32 texture_get_built_in(BuiltInTextureEnum texture);

#endif //ACORN_TEXTURE_H
