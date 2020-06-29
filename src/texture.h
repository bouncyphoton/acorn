#ifndef ACORN_TEXTURE_H
#define ACORN_TEXTURE_H

#include "types.h"
#include <GL/gl3w.h>
#include <string>

enum class BuiltInTextureEnum {
    MISSING, BLACK, WHITE, NORMAL
};

// TODO: binding from texture class? and other opengl funcs?

class Texture {
public:
    /// Init a texture from a file
    void init(const std::string &path);

    /// Init a 2D texture with data
    void init2d(GLenum texture_format, s32 width, s32 height, GLenum data_type, u8 *data, GLenum data_format);

    /// Init a cubemap texture with data
    void initCubemap(GLenum texture_format, s32 width, s32 height, GLenum data_type, void **data, GLenum data_format);

    /// Clean up
    void destroy();

    u32 id;
};

#endif //ACORN_TEXTURE_H
