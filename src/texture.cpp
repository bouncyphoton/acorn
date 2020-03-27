#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <GL/gl3w.h>
#include <unordered_map>

static std::unordered_map<std::string, u32> texture_map;
static u32 texture_black = 0;
static u32 texture_white = 0;
static u32 texture_missing = 0;

static u32 texture_generate(s32 width, s32 height, u8 *data, GLenum internal_format, GLenum format) {
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

bool textures_init() {
    stbi_set_flip_vertically_on_load(1);

    u8 black[4] = {0, 0, 0, 255};
    texture_black = texture_generate(1, 1, black, GL_RGBA, GL_RGBA);

    u8 white[4] = {0, 0, 0, 255};
    texture_white = texture_generate(1, 1, white, GL_RGBA, GL_RGBA);

    u8 missing[4 * 4] = {255, 0, 255, 255,
                         0, 0, 0, 255,
                         255, 0, 255, 255,
                         0, 0, 0, 255};
    texture_missing = texture_generate(2, 2, missing, GL_RGBA, GL_RGBA);

    return texture_black != 0 && texture_white != 0 && texture_missing != 0;
}

void textures_shutdown() {
    glDeleteTextures(1, &texture_black);
    glDeleteTextures(1, &texture_white);
    glDeleteTextures(1, &texture_missing);
    for (auto& t : texture_map) {
        glDeleteTextures(1, &t.second);
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

    // generate opengl texture and cache
    u32 texture = texture_generate(width, height, data, GL_RGBA, GL_RGBA);
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
        default:
            fprintf(stderr, "[error] failed to get built in texture %d\n", (int)texture);
        case BuiltInTextureEnum::MISSING:
            return texture_missing;
    }
}
