#ifndef ACORN_RESOURCE_MANAGER_H
#define ACORN_RESOURCE_MANAGER_H

#include "types.h"
#include "graphics/model.h"
#include "graphics/texture.h"
#include <unordered_map>
#include <string>

enum class BuiltInTextureEnum {
    MISSING, BLACK, WHITE, NORMAL
};

enum class BuiltInModelEnum {
    PLANE
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    /// Get a model and if not loaded, load
    Model *getModel(const std::string &path);

    /// Get a texture and if not loaded, load
    Texture *getTexture(const std::string &path);

    /// Get a texture and split image channels into separate textures. Pointers can be null
    void getTextureSplitComponents(const std::string &path, Texture **texture_red, Texture **texture_green,
                                   Texture **texture_blue, Texture **texture_alpha);

    /// Get a built in texture
    Texture *getBuiltInTexture(BuiltInTextureEnum tex);

    /// Get a built in model
    Model *getBuiltInModel(BuiltInModelEnum model);

private:
    void init();

    void destroy();

    std::unordered_map<std::string, Model *> m_models;
    std::unordered_map<std::string, Texture *> m_textures;
    Texture2D m_textureBlack;   // (0, 0, 0)
    Texture2D m_textureWhite;   // (255, 255, 255)
    Texture2D m_textureNormal;  // (127, 127, 255)
    Texture2D m_textureMissing; // (0, 0, 0) (255, 0, 255) pattern
    Model *m_modelPlane; // Unit plane [-1, 1] with a +y normal
};

#endif //ACORN_RESOURCE_MANAGER_H
