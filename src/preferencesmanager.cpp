#include "preferencesmanager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <libvideo2x/logger_manager.h>
#include <libvideo2x/version.h>

#include "utils.h"

PreferencesManager::PreferencesManager() {}

bool PreferencesManager::initializeConfigPath()
{
    std::optional<std::filesystem::path> configDir = getConfigDir();
    if (!configDir.has_value()) {
        return false;
    }

#ifdef _WIN32
    m_configFilePath = configDir.value() / L"video2x-qt6.json";
#else
    m_configFilePath = configDir.value() / "video2x-qt6.json";
#endif

    return true;
}

bool PreferencesManager::loadPreferences()
{
    if (!initializeConfigPath()) {
        video2x::logger()->warn("Configuration path initialization failed.");
        return {};
    }

    // Set the preferences to default values
    m_preferences = Video2XPreferences{};

    if (!std::filesystem::is_regular_file(m_configFilePath)) {
        video2x::logger()->info("No existing configuration file found.");
        return true;
    }

#ifdef _WIN32
    QFile file(QString::fromStdWString(m_configFilePath.wstring()));
#else
    QFile file(QString::fromStdString(m_configFilePath.string()));
#endif
    if (!file.open(QIODevice::ReadOnly)) {
        video2x::logger()->warn("Failed to open configuration file for reading.");
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        video2x::logger()->warn("Failed to parse configuration file as valid JSON.");
        return false;
    }

    QJsonObject jsonObject = jsonDoc.object();

    // Parse check updates
    if (jsonObject.contains(CHECK_UPDATES_KEY) && jsonObject[CHECK_UPDATES_KEY].isBool()) {
        m_preferences.checkUpdates = jsonObject[CHECK_UPDATES_KEY].toBool();
    }

    // Parse translation
    if (jsonObject.contains(TRANSLATION_KEY) && jsonObject[TRANSLATION_KEY].isString()) {
        m_preferences.translation = jsonObject[TRANSLATION_KEY].toString();
    }

    // Parse action when done
    if (jsonObject.contains(ACTION_WHEN_DONE_KEY) && jsonObject[ACTION_WHEN_DONE_KEY].isString()) {
        QString actionWhenDoneString = jsonObject[ACTION_WHEN_DONE_KEY].toString();
        if (actionWhenDoneString == "do_nothing") {
            m_preferences.actionWhenDone = Video2XPreferences::ActionWhenDone::DoNothing;
        } else if (actionWhenDoneString == "shutdown") {
            m_preferences.actionWhenDone = Video2XPreferences::ActionWhenDone::Shutdown;
        } else if (actionWhenDoneString == "sleep") {
            m_preferences.actionWhenDone = Video2XPreferences::ActionWhenDone::Sleep;
        } else if (actionWhenDoneString == "hibernate") {
            m_preferences.actionWhenDone = Video2XPreferences::ActionWhenDone::Hibernate;
        }
    }

    // Parse auto show stats
    if (jsonObject.contains(AUTO_SHOW_STATS_KEY) && jsonObject[AUTO_SHOW_STATS_KEY].isBool()) {
        m_preferences.autoShowStats = jsonObject[AUTO_SHOW_STATS_KEY].toBool();
    }

    // Parse remove finished tasks
    if (jsonObject.contains(REMOVE_FINISHED_TASKS_KEY) && jsonObject[REMOVE_FINISHED_TASKS_KEY].isBool()) {
        m_preferences.removeFinishedTasks = jsonObject[REMOVE_FINISHED_TASKS_KEY].toBool();
    }

    return true;
}

bool PreferencesManager::savePreferences()
{
    if (!initializeConfigPath()) {
        video2x::logger()->warn("Configuration path initialization failed.");
        return false;
    }

    // Ensure the directory exists only on save
    std::filesystem::path configDir = m_configFilePath.parent_path();
    if (!std::filesystem::exists(configDir)) {
        try {
            video2x::logger()->info("Creating configuration directory.");
            std::filesystem::create_directories(configDir);
        } catch (const std::filesystem::filesystem_error &e) {
            video2x::logger()->warn("Failed to create configuration directories: {}.", e.what());
            return false;
        }
    }

    QJsonObject jsonObject;
    jsonObject[CONFIG_VERSION_KEY] = LIBVIDEO2X_VERSION_STRING;
    
    // Save check updates
    if (m_preferences.checkUpdates.has_value()){
        jsonObject[CHECK_UPDATES_KEY] = m_preferences.checkUpdates.value();
    }

    // Save translation
    jsonObject[TRANSLATION_KEY] = m_preferences.translation;

    // Save action when done
    switch (m_preferences.actionWhenDone) {
    case Video2XPreferences::ActionWhenDone::DoNothing:
        jsonObject[ACTION_WHEN_DONE_KEY] = "do_nothing";
        break;
    case Video2XPreferences::ActionWhenDone::Shutdown:
        jsonObject[ACTION_WHEN_DONE_KEY] = "shutdown";
        break;
    case Video2XPreferences::ActionWhenDone::Sleep:
        jsonObject[ACTION_WHEN_DONE_KEY] = "sleep";
        break;
    case Video2XPreferences::ActionWhenDone::Hibernate:
        jsonObject[ACTION_WHEN_DONE_KEY] = "hibernate";
        break;
    }

    // Save auto show stats
    jsonObject[AUTO_SHOW_STATS_KEY] = m_preferences.autoShowStats;

    // Save remove finished tasks
    jsonObject[REMOVE_FINISHED_TASKS_KEY] = m_preferences.removeFinishedTasks;

    QJsonDocument jsonDoc(jsonObject);

#ifdef _WIN32
    QFile file(QString::fromStdWString(m_configFilePath.wstring()));
#else
    QFile file(QString::fromStdString(m_configFilePath.string()));
#endif
    if (!file.open(QIODevice::WriteOnly)) {
        video2x::logger()->warn("Failed to open configuration file for writing.");
        return false;
    }

    file.write(jsonDoc.toJson());
    file.close();
    return true;
}

Video2XPreferences &PreferencesManager::getPreferences()
{
    return m_preferences;
}

void PreferencesManager::setPreferences(Video2XPreferences &pref)
{
    m_preferences = pref;
}

void PreferencesManager::resetPreferences()
{
    m_preferences = Video2XPreferences{};
    savePreferences();
}
