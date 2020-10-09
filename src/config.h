#ifndef ACORN_CONFIG_H
#define ACORN_CONFIG_H

struct ConfigData {
    bool debugLoggingEnabled = true;
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
