#include "resource_manager.h"
#include "core.h"
#include <stb_image.h>

void ResourceManager::init() {
    stbi_set_flip_vertically_on_load(1);

    u8 black[4] = {0, 0, 0, 255};
    m_textureBlack = new Texture();
    m_textureBlack->init2d(GL_RGBA, 1, 1, GL_UNSIGNED_BYTE, black, GL_RGBA);

    u8 white[4] = {0, 0, 0, 255};
    m_textureWhite = new Texture();
    m_textureWhite->init2d(GL_RGBA, 1, 1, GL_UNSIGNED_BYTE, white, GL_RGBA);

    u8 normal[4] = {127, 127, 255, 255};
    m_textureNormal = new Texture();
    m_textureNormal->init2d(GL_RGBA, 1, 1, GL_UNSIGNED_BYTE, normal, GL_RGBA);

    u8 missing[4 * 4] = {255, 0, 255, 255,
                         0, 0, 0, 255,
                         255, 0, 255, 255,
                         0, 0, 0, 255};
    m_textureMissing = new Texture();
    m_textureMissing->init2d(GL_RGBA, 2, 2, GL_UNSIGNED_BYTE, missing, GL_RGBA);
}

void ResourceManager::destroy() {
    for (auto &model : m_models) {
        model.second->destroy();
        delete model.second;
    }

    for (auto &texture : m_textures) {
        texture.second->destroy();
        delete texture.second;
    }

    // TODO: I don't like the new, init, destroy, delete cycle - maybe move to constructors that fail for most things?
    m_textureBlack->destroy();
    delete m_textureBlack;
    m_textureWhite->destroy();
    delete m_textureWhite;
    m_textureNormal->destroy();
    delete m_textureNormal;
    m_textureMissing->destroy();
    delete m_textureMissing;
}

Model *ResourceManager::getModel(const std::string &path) {
    // See if model is already loaded
    auto it = m_models.find(path);
    if (it != m_models.end()) {
        return it->second;
    }

    // Try to load model
    core->info("Loading model '" + path + "'");
    Model *model = new Model();
    model->init(path);
    m_models.emplace(path, model);
    return model;
}

Texture *ResourceManager::getTexture(const std::string &path) {
    // See if texture is already loaded
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    // Try to load texture
    core->info("Loading texture '" + path + "'");
    Texture *texture = new Texture();
    texture->init(path);
    m_textures.emplace(path, texture);
    return texture;
}

Texture *ResourceManager::getBuiltInTexture(BuiltInTextureEnum default_tex) {
    switch (default_tex) {
        case BuiltInTextureEnum::BLACK:
            return m_textureBlack;
        case BuiltInTextureEnum::WHITE:
            return m_textureWhite;
        case BuiltInTextureEnum::NORMAL:
            return m_textureNormal;
        default:
            core->warn("Unhandled case for getting a built in texture: " + std::to_string(static_cast<int>(default_tex)));
        case BuiltInTextureEnum::MISSING:
            return m_textureMissing;
    }
}
