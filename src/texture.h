#ifndef ACORN_TEXTURE_H
#define ACORN_TEXTURE_H

#include "types.h"
#include <GL/gl3w.h>
#include <string>

class Texture {
public:
    Texture();
    Texture(Texture &&other);
    Texture &operator=(Texture &&other);
    ~Texture();

    /// Init a texture from a file
    void init(const std::string &path);

    /// Init a 2D texture with data
    void init2D(GLenum texture_format, s32 width, s32 height, GLenum data_type, u8 *data, GLenum data_format);

    /// Init a cubemap texture with data
    void initCubemap(GLenum texture_format, s32 width, s32 height, GLenum data_type, void **data, GLenum data_format);

    void bindTex2D(u32 unit);

    void bindCubemap(u32 unit);

    u32 getId() const {
        return m_id;
    }

private:
    u32 m_id;
};

#endif //ACORN_TEXTURE_H
