#include "ibl_probe.h"
#include "constants.h"
#include <vector>

IblProbe::IblProbe(glm::vec3 position)
    : m_isDirty(true), m_position(position) {
    clearToBlack();
}

void IblProbe::clearToBlack() {
    m_environmentMap.setImage(consts::IBL_PROBE_CAPTURE_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_diffuseIrradianceCubemap.setImage(consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_prefilteredCubemap.setImage(consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE, TextureFormatEnum::RGB16F);
    m_numPrefilteredMipmapLevels = consts::NUM_PREFILTERED_MIP_LEVELS;
}

void IblProbe::clearToWhite() {
    int highest = glm::max(consts::IBL_PROBE_CAPTURE_TEXTURE_SIZE,
                           glm::max(consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE, consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE));
    std::vector<glm::vec3> white(highest * highest, glm::vec3(1));

    void *data[6];
    for (int i = 0; i < 6; ++i) {
        data[i] = white.data();
    }

    m_environmentMap.setImage(consts::IBL_PROBE_CAPTURE_TEXTURE_SIZE, TextureFormatEnum::RGB16F, data);
    m_diffuseIrradianceCubemap.setImage(consts::DIFFUSE_IRRADIANCE_TEXTURE_SIZE, TextureFormatEnum::RGB16F, data);
    m_prefilteredCubemap.setImage(consts::PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE, TextureFormatEnum::RGB16F, data);
    m_numPrefilteredMipmapLevels = consts::NUM_PREFILTERED_MIP_LEVELS;
}
