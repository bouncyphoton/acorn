#include "texture.h"
#include "core.h"

#include <GL/gl3w.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include <stb_image.h>

Texture::Texture() {
    glGenTextures(1, &m_id);
    core->debug("Texture::Texture() - #" + std::to_string(m_id));
}

Texture::Texture(Texture &&other)
        : m_id(other.m_id) {
    other.m_id = 0;
}

Texture &Texture::operator=(Texture &&other) {
    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Texture::~Texture() {
    core->debug("Texture::~Texture() - #" + std::to_string(m_id));
    glDeleteTextures(1, &m_id);
}

void Texture::init(const std::string &path) {
    // TODO: read from some sort of asset metadata file
    // would contain: data format (RGBA, etc.), texture format (2d, cubemap, etc.), is hdr, etc.
    GLenum textureFormat = GL_RGBA;
    GLenum dataFormat = GL_RGBA;


    // load image data from file
    s32 width, height, channels;
    u8 *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        core->fatal("Failed to load image '" + path + "'\n" + stbi_failure_reason());
    }

    // init w/ data
    init2D(textureFormat, width, height, GL_UNSIGNED_BYTE, data, dataFormat);

    // free image data
    stbi_image_free(data);
}

void Texture::init2D(GLenum texture_format, s32 width, s32 height, GLenum data_type, u8 *data, GLenum data_format) {
    s32 previouslyBound;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previouslyBound);

    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, 0, data_format, data_type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, previouslyBound);
}

void Texture::initCubemap(GLenum texture_format, s32 width, s32 height, GLenum data_type,
                          void **data, GLenum data_format) {
    s32 previouslyBound;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previouslyBound);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texture_format, width, height, 0, data_format,
                     data_type, data ? data[i] : nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, previouslyBound);
}

void Texture::bindTex2D(u32 unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::bindCubemap(u32 unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}
