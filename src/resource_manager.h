#ifndef ACORN_RESOURCE_MANAGER_H
#define ACORN_RESOURCE_MANAGER_H

#include "types.h"
#include "model.h"
#include "texture.h"
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    void init();

    void destroy();

    /// Get a model and if not loaded, load
    Model *getModel(const std::string &path);

    /// Get a texture and if not loaded, load
    Texture *getTexture(const std::string &path);

    /// Get a built in texture
    Texture *getBuiltInTexture(BuiltInTextureEnum default_tex);

private:
    std::unordered_map<std::string, Model *> m_models;
    std::unordered_map<std::string, Texture *> m_textures;
    Texture *m_textureBlack;   // (0, 0, 0)
    Texture *m_textureWhite;   // (255, 255, 255)
    Texture *m_textureNormal;  // (127, 127, 255)
    Texture *m_textureMissing; // (0, 0, 0) (255, 0, 255) pattern
};

#endif //ACORN_RESOURCE_MANAGER_H
