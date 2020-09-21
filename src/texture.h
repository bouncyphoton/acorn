#ifndef ACORN_TEXTURE_H
#define ACORN_TEXTURE_H

#include "types.h"
#include <GL/gl3w.h>
#include <string>

enum class TextureFormatEnum {
    RGB8, RGBA8, RG16F, RGB16F, RGBA16F, RGB32F, RGBA32F
};

class Texture {
public:
    Texture();
    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;
    virtual ~Texture();

    virtual void bind(u32 unit) = 0;

    u32 getId() const {
        return m_id;
    }

private:
    u32 m_id;
};

class Texture2D : public Texture {
public:
    /// Inherit constructors
    using Texture::Texture;

    void bind(u32 unit) override;

    void setImage(int width, int height, TextureFormatEnum format, void *data = nullptr);

    u32 getWidth() const {
        return m_width;
    }

    u32 getHeight() const {
        return m_height;
    }

private:
    u32 m_width;
    u32 m_height;
};

class TextureCubemap : public Texture {
public:
    /// Inherit constructors
    using Texture::Texture;

    void bind(u32 unit) override;

    void setImage(int side_length, TextureFormatEnum format, void *data[6] = nullptr);

    u32 getSideLength() const {
        return m_sideLength;
    }

private:
    u32 m_sideLength;
};

#endif //ACORN_TEXTURE_H
