#pragma once

#include "graphics/texture.h"
#include <glm/glm.hpp>

class IblProbe {
public:
    // TODO: probably will have a range of influence too
    IblProbe(glm::vec3 position);

    void clearToBlack();

    void clearToWhite();

    void unsetDirty() {
        m_isDirty = false;
    }

    TextureCubemap &getEnvironmentMap() {
        return m_environmentMap;
    }

    const TextureCubemap &getEnvironmentMap() const {
        return m_environmentMap;
    }

    const TextureCubemap &getDiffuseIrradianceCubemap() const {
        return m_diffuseIrradianceCubemap;
    }

    const TextureCubemap &getPrefilteredCubemap() const {
        return m_prefilteredCubemap;
    }

    s32 getNumPrefilteredMipmapLevels() const {
        return m_numPrefilteredMipmapLevels;
    }

    bool isDirty() const {
        return m_isDirty;
    }

    glm::vec3 getPosition() const {
        return m_position;
    }

private:
    bool m_isDirty;
    glm::vec3 m_position;

    TextureCubemap m_environmentMap;
    TextureCubemap m_diffuseIrradianceCubemap;
    TextureCubemap m_prefilteredCubemap;
    s32 m_numPrefilteredMipmapLevels;
};
