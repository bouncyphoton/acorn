#include "texture.h"
#include "core.h"
#include "utils.h"
#include <GL/gl3w.h>

Texture::Texture() {
    glGenTextures(1, &m_id);
    core->debug("Texture::Texture() - #" + std::to_string(m_id));
}

Texture::Texture(Texture &&other) noexcept
        : m_id(other.m_id) {
    other.m_id = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Texture::~Texture() {
    core->debug("Texture::~Texture() - #" + std::to_string(m_id));
    glDeleteTextures(1, &m_id);
}

void Texture2D::bind(u32 unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, getId());
}

void Texture2D::setImage(int width, int height, TextureFormatEnum format, void *data) {
    s32 previouslyBound;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previouslyBound);

    u32 textureFormat, dataFormat, dataType;
    utils::get_format_info(format, &textureFormat, &dataFormat, &dataType);

    glBindTexture(GL_TEXTURE_2D, getId());
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, dataFormat, dataType, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, previouslyBound);
}

void TextureCubemap::bind(u32 unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, getId());
}

void TextureCubemap::setImage(int side_length, TextureFormatEnum format, void **data) {
    s32 previouslyBound;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previouslyBound);

    u32 textureFormat, dataFormat, dataType;
    utils::get_format_info(format, &textureFormat, &dataFormat, &dataType);

    glBindTexture(GL_TEXTURE_CUBE_MAP, getId());
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textureFormat, side_length, side_length, 0, dataFormat,
                     dataType, data ? data[i] : nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, previouslyBound);
}
