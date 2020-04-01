#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <GL/gl3w.h>
#include <unordered_map>

static std::unordered_map<std::string, u32> texture_map;
static u32 texture_black = 0;
static u32 texture_white = 0;
static u32 texture_missing = 0;
static u32 texture_normal = 0;

u32 texture_create(GLenum texture_format, s32 width, s32 height, u8 *data, GLenum data_format) {
    s32 previously_bound;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previously_bound);

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, previously_bound);
    return texture;
}

u32 texture_create(GLenum texture_format, u32 width, u32 height) {
    return texture_create(texture_format, width, height, nullptr, texture_format);
}

void texture_destroy(u32 texture) {
    glDeleteTextures(1, &texture);
}

bool textures_init() {
    stbi_set_flip_vertically_on_load(1);

    u8 black[4] = {0, 0, 0, 255};
    texture_black = texture_create(GL_RGBA, 1, 1, black, GL_RGBA);

    u8 white[4] = {0, 0, 0, 255};
    texture_white = texture_create(GL_RGBA, 1, 1, white, GL_RGBA);

    u8 missing[4 * 4] = {255, 0, 255, 255,
                         0, 0, 0, 255,
                         255, 0, 255, 255,
                         0, 0, 0, 255};
    texture_missing = texture_create(GL_RGBA, 2, 2, missing, GL_RGBA);

    u8 normal[4] = {127, 127, 255, 255};
    texture_normal = texture_create(GL_RGBA, 1, 1, normal, GL_RGBA);

    return texture_black != 0 && texture_white != 0 && texture_missing != 0 && texture_normal != 0;
}

void textures_shutdown() {
    texture_destroy(texture_black);
    texture_destroy(texture_white);
    texture_destroy(texture_missing);
    for (auto &t : texture_map) {
        texture_destroy(t.second);
    }
}

u32 texture_get(const char *path, BuiltInTextureEnum default_tex) {
    // get cached texture if possible
    auto it = texture_map.find(path);
    if (it != texture_map.end()) {
        return it->second;
    }

    // load image data from file
    s32 width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    u8 *data = stbi_load(path, &width, &height, &channels, 4);
    if (!data) {
        fprintf(stderr, "[error] failed to load image \"%s\"\n", path);
        return texture_get_built_in(default_tex);
    }

    // TODO: don't make everything RGBA
    // generate opengl texture and cache
    u32 texture = texture_create(GL_RGBA, width, height, data, GL_RGBA);
    texture_map.emplace(path, texture);

    // free image data
    stbi_image_free(data);

    return texture;
}

u32 texture_get_built_in(BuiltInTextureEnum texture) {
    switch (texture) {
        case BuiltInTextureEnum::BLACK:
            return texture_black;
        case BuiltInTextureEnum::WHITE:
            return texture_white;
        case BuiltInTextureEnum::NORMAL:
            return texture_normal;
        default:
            fprintf(stderr, "[error] failed to get built in texture %d\n", (int) texture);
        case BuiltInTextureEnum::MISSING:
            return texture_missing;
    }
}

// TODO: check performance
void texture_get_dimensions(u32 texture, u32 miplevel, s32 *width, s32 *height) {
    if (texture == 0 || width == nullptr || height == nullptr) {
        return;
    }

    s32 previously_bound;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previously_bound);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, height);

    glBindTexture(GL_TEXTURE_2D, previously_bound);
}
