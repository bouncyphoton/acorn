#ifndef ACORN_CONFIG_H
#define ACORN_CONFIG_H

#include "types.h"

struct ConfigData {
    bool debugLoggingEnabled = true;
    u32 width = 1280;
    u32 height = 720;
    u32 vsyncNumSwapFrames = 0;
};

class Config {
public:
    Config();

    const ConfigData &getConfigData() const {
        return m_configData;
    }

private:
    ConfigData m_configData;
};

#endif //ACORN_CONFIG_H
