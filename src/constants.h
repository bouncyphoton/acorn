#ifndef ACORN_CONSTANTS_H
#define ACORN_CONSTANTS_H

namespace consts {
constexpr const char *APP_NAME = "acorn";
constexpr u32 OPENGL_VERSION_MAJOR = 3;
constexpr u32 OPENGL_VERSION_MINOR = 3;

// Renderer
constexpr u32 DIFFUSE_IRRADIANCE_TEXTURE_SIZE = 32;
constexpr u32 PREFILTERED_ENVIRONMENT_MAP_TEXTURE_SIZE = 128;
constexpr u32 BRDF_LUT_TEXTURE_SIZE = 512;
}

#endif //ACORN_CONSTANTS_H
