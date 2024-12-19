#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <optional>

#include <QString>
#include <filesystem>

#define CHECKUPGRADES_KEY "checkUpgrades"
#define TRANSLATION_KEY "translation"

struct Video2XConfig
{
    std::optional<bool> checkUpgrades = std::nullopt;
    std::optional<QString> translation = std::nullopt;
};

class ConfigManager
{
public:
    ConfigManager();
    bool initializeConfigPath();
    Video2XConfig loadConfig();
    bool saveConfig(const Video2XConfig &config);
    bool removeConfig();

private:
    std::filesystem::path m_configFilePath;
};

#endif // CONFIGMANAGER_H
