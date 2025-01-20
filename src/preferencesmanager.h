#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <filesystem>

#include <QString>

#define CONFIG_VERSION_KEY "configVersion"
#define CHECK_UPDATES_KEY "checkUpdates"
#define TRANSLATION_KEY "translation"
#define ACTION_WHEN_DONE_KEY "actionWhenDone"
#define REMOVE_FINISHED_TASKS_KEY "removeFinishedTasks"

struct Video2XPreferences
{
    enum class ActionWhenDone { DoNothing, Shutdown, Sleep, Hibernate };

    // Which available type of update to check on startup
    std::optional<bool> checkUpdates = std::nullopt;

    // The translation to load on startup
    QString translation = "system";

    // The action to perform after all tasks are processed
    ActionWhenDone actionWhenDone = ActionWhenDone::DoNothing;

    // Whether finished tasks should be removed from the task queue
    bool removeFinishedTasks = false;
};

class PreferencesManager
{
public:
    PreferencesManager();
    bool initializeConfigPath();
    bool loadPreferences();
    bool savePreferences();
    Video2XPreferences &getPreferences();
    void setPreferences(Video2XPreferences &pref);
    void resetPreferences();

private:
    std::filesystem::path m_configFilePath;
    Video2XPreferences m_preferences;
};

#endif // PREFERENCESMANAGER_H
