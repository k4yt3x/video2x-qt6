#include "configmanager.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

ConfigManager::ConfigManager() {}

bool ConfigManager::initializeConfigPath()
{
#ifdef _WIN32
    wchar_t localAppData[1024];
    size_t size = 0;
    if (_wgetenv_s(&size, localAppData, 1024, L"LOCALAPPDATA") == 0 && size > 0) {
        m_configFilePath = std::filesystem::path(localAppData) / L"video2x-qt6/video2x-qt6.json";
    } else {
        qWarning() << "Failed to retrieve LOCALAPPDATA environment variable";
        return false;
    }
#else
    const char *xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome) {
        m_configFilePath = std::filesystem::path(xdgConfigHome) / "video2x-qt6/video2x-qt6.json";
    } else {
        const char *homeDir = std::getenv("HOME");
        if (homeDir) {
            m_configFilePath = std::filesystem::path(homeDir)
                               / ".config/video2x-qt6/video2x-qt6.json";
        } else {
            qWarning() << "Failed to retrieve HOME environment variable";
            return false;
        }
    }
#endif

    return true;
}

Video2XConfig ConfigManager::loadConfig()
{
    if (!initializeConfigPath()) {
        qWarning() << "Config path initialization failed";
        return {};
    }

    Video2XConfig config;

    if (!std::filesystem::is_regular_file(m_configFilePath)) {
        qDebug() << "The config file does not exist; nothing to load";
        return config;
    }

    QFile file(QString::fromStdWString(m_configFilePath.wstring()));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file for reading";
        return config;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Failed to parse config file as JSON";
        return config;
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (jsonObject.contains(CHECKUPGRADES_KEY) && jsonObject[CHECKUPGRADES_KEY].isBool()) {
        config.checkUpgrades = jsonObject[CHECKUPGRADES_KEY].toBool();
    }
    if (jsonObject.contains(TRANSLATION_KEY) && jsonObject[TRANSLATION_KEY].isString()) {
        config.translation = jsonObject[TRANSLATION_KEY].toString();
    }

    return config;
}

bool ConfigManager::saveConfig(const Video2XConfig &config)
{
    if (!initializeConfigPath()) {
        qWarning() << "Config path initialization failed";
        return false;
    }

    // Ensure the directory exists only on save
    std::filesystem::path configDir = m_configFilePath.parent_path();
    if (!std::filesystem::exists(configDir)) {
        try {
            qDebug() << "Creating the config directory";
            std::filesystem::create_directories(configDir);
        } catch (const std::filesystem::filesystem_error &e) {
            qWarning() << "Failed to create config directories:" << e.what();
            return false;
        }
    }

    QJsonObject jsonObject;
    if (config.checkUpgrades.has_value()) {
        jsonObject[CHECKUPGRADES_KEY] = config.checkUpgrades.value();
    }
    if (config.translation.has_value()) {
        jsonObject[TRANSLATION_KEY] = config.translation.value();
    }

    QJsonDocument jsonDoc(jsonObject);

    QFile file(QString::fromStdWString(m_configFilePath.wstring()));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing";
        return false;
    }

    file.write(jsonDoc.toJson());
    file.close();
    return true;
}

bool ConfigManager::removeConfig()
{
    if (!initializeConfigPath()) {
        qWarning() << "Config path initialization failed";
        return false;
    }

    try {
        // Remove the config file
        if (std::filesystem::exists(m_configFilePath)
            && std::filesystem::is_regular_file(m_configFilePath)) {
            std::filesystem::remove(m_configFilePath);
        } else {
            qWarning() << "Config file does not exist or is not a regular file";
            return false;
        }

        // Remove the parent directory if it is empty
        std::filesystem::path configDir = m_configFilePath.parent_path();
        if (std::filesystem::exists(configDir) && std::filesystem::is_empty(configDir)) {
            std::filesystem::remove(configDir);
        }

        return true;
    } catch (const std::filesystem::filesystem_error &e) {
        qWarning() << "Failed to remove config file or directory:" << e.what();
        return false;
    }
}
