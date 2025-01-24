#include "utils.h"

#include <QFileInfo>
#include <QMap>
#include <QMimeDatabase>

#ifdef _WIN32
#include <QSettings>
#include <Windows.h>
#include <powrprof.h>
// #pragma comment(lib, "PowrProf.lib")
#else
#include <QDBusInterface>
#include <QDBusReply>
#endif

#include <libvideo2x/logger_manager.h>

#ifdef _WIN32
bool isVCRuntimeRequirementMet()
{
    const QString registryKey
        = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64";
    const DWORD requiredMajor = 14;
    const DWORD requiredMinor = 42;

    QSettings reg(registryKey, QSettings::NativeFormat);

    // Check if the registry key exists
    if (!reg.contains("Major") || !reg.contains("Minor")) {
        return false;
    }

    DWORD installedMajor = reg.value("Major").toUInt();
    DWORD installedMinor = reg.value("Minor").toUInt();

    // Check if the installed version meets the requirement
    if (installedMajor > requiredMajor
        || (installedMajor == requiredMajor && installedMinor >= requiredMinor)) {
        return true;
    }

    return false;
}
#endif

// Function to shut down the system
void systemShutdown()
{
#ifdef _WIN32
    if (!InitiateSystemShutdownExW(nullptr, nullptr, 0, TRUE, FALSE, SHTDN_REASON_MAJOR_OTHER)) {
        video2x::logger()->error("Failed to shutdown the system. Error code: {}.", GetLastError());
    } else {
        video2x::logger()->info("System shutdown initiated successfully.");
    }
#else
    QDBusInterface interface("org.freedesktop.login1",
                             "/org/freedesktop/login1",
                             "org.freedesktop.login1.Manager",
                             QDBusConnection::systemBus());

    if (!interface.isValid()) {
        video2x::logger()->error("Failed to connect to systemd for shutdown: {}.",
                                 QDBusConnection::systemBus().lastError().message().toStdString());
        return;
    }

    QDBusReply<void> reply = interface.call("PowerOff", true); // true = interactive
    if (!reply.isValid()) {
        video2x::logger()->error("Failed to initiate shutdown: {}.",
                                 reply.error().message().toStdString());
    } else {
        video2x::logger()->info("System shutdown initiated successfully.");
    }
#endif
}

// Function to put the system to sleep
void systemSleep()
{
#ifdef _WIN32
    if (!SetSuspendState(FALSE, FALSE, FALSE)) {
        video2x::logger()->error("Failed to put the system to sleep. Error code: {}.",
                                 GetLastError());
    } else {
        video2x::logger()->info("System sleep initiated successfully.");
    }
#else
    QDBusInterface interface("org.freedesktop.login1",
                             "/org/freedesktop/login1",
                             "org.freedesktop.login1.Manager",
                             QDBusConnection::systemBus());

    if (!interface.isValid()) {
        video2x::logger()->error("Failed to connect to systemd for suspend: {}.",
                                 QDBusConnection::systemBus().lastError().message().toStdString());
        return;
    }

    QDBusReply<void> reply = interface.call("Suspend", true); // true = interactive
    if (!reply.isValid()) {
        video2x::logger()->error("Failed to initiate sleep: {}.",
                                 reply.error().message().toStdString());
    } else {
        video2x::logger()->info("System sleep initiated successfully.");
    }
#endif
}

// Function to hibernate the system
void systemHibernate()
{
#ifdef _WIN32
    if (!SetSuspendState(TRUE, FALSE, FALSE)) {
        video2x::logger()->error("Failed to hibernate the system. Error code: {}.", GetLastError());
    } else {
        video2x::logger()->info("System hibernation initiated successfully.");
    }
#else
    QDBusInterface interface("org.freedesktop.login1",
                             "/org/freedesktop/login1",
                             "org.freedesktop.login1.Manager",
                             QDBusConnection::systemBus());

    if (!interface.isValid()) {
        video2x::logger()->error("Failed to connect to systemd for hibernate: {}.",
                                 QDBusConnection::systemBus().lastError().message().toStdString());
        return;
    }

    QDBusReply<void> reply = interface.call("Hibernate", true); // true = interactive
    if (!reply.isValid()) {
        video2x::logger()->error("Failed to initiate hibernate: {}.",
                                 reply.error().message().toStdString());
    } else {
        video2x::logger()->info("System hibernation initiated successfully.");
    }
#endif
}

std::optional<std::filesystem::path> getConfigDir()
{
#ifdef _WIN32
    wchar_t localAppData[1024];
    size_t size = 0;
    if (_wgetenv_s(&size, localAppData, 1024, L"LOCALAPPDATA") == 0 && size > 0) {
        return std::filesystem::path(localAppData) / L"video2x-qt6";
    } else {
        video2x::logger()->warn("Failed to retrieve LOCALAPPDATA environment variable.");
        return std::nullopt;
    }
#else
    const char *xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome) {
        return std::filesystem::path(xdgConfigHome) / "video2x-qt6";
    } else {
        const char *homeDir = std::getenv("HOME");
        if (homeDir) {
            return std::filesystem::path(homeDir) / ".config/video2x-qt6";
        } else {
            video2x::logger()->warn("Failed to retrieve HOME environment variable.");
            return std::nullopt;
        }
    }
#endif
}

std::optional<QString> findAnime4kFileNameByDisplayName(const QString &displayName)
{
    static const QMap<QString, QString> anime4kDisplayToFileNameMap = {
        {"Anime4K v4 Mode A", "anime4k-v4-a"},
        {"Anime4K v4 Mode B", "anime4k-v4-b"},
        {"Anime4K v4 Mode C", "anime4k-v4-c"},
        {"Anime4K v4 Mode A+A", "anime4k-v4-a+a"},
        {"Anime4K v4 Mode B+B", "anime4k-v4-b+b"},
        {"Anime4K v4 Mode C+A", "anime4k-v4-c+a"},
        {"Anime4K v4.1 GAN", "anime4k-v4.1-gan"},
    };

    if (anime4kDisplayToFileNameMap.contains(displayName)) {
        return anime4kDisplayToFileNameMap.value(displayName);
    }
    return std::nullopt;
}

std::optional<QString> findAnime4kDisplayNameByFileName(const QString &fileName)
{
    static const QMap<QString, QString> anime4kFileNameToDisplayNameMap = {
        {"anime4k-v4-a", "Anime4K v4 Mode A"},
        {"anime4k-v4-b", "Anime4K v4 Mode B"},
        {"anime4k-v4-c", "Anime4K v4 Mode C"},
        {"anime4k-v4-a+a", "Anime4K v4 Mode A+A"},
        {"anime4k-v4-b+b", "Anime4K v4 Mode B+B"},
        {"anime4k-v4-c+a", "Anime4K v4 Mode C+A"},
        {"anime4k-v4.1-gan", "Anime4K v4.1 GAN"},
    };

    if (anime4kFileNameToDisplayNameMap.contains(fileName)) {
        return anime4kFileNameToDisplayNameMap.value(fileName);
    }
    return std::nullopt;
}

std::optional<QString> findRifeModelNameByDisplayName(const QString &displayName)
{
    static const QMap<QString, QString> rifeDisplayToFileNameMap = {
        {"RIFE", "rife"},
        {"RIFE HD", "rife-HD"},
        {"RIFE UHD", "rife-UHD"},
        {"RIFE Anime", "rife-anime"},
        {"RIFE v2", "rife-v2"},
        {"RIFE v2.3", "rife-v2.3"},
        {"RIFE v2.4", "rife-v2.4"},
        {"RIFE v3.0", "rife-v3.0"},
        {"RIFE v3.1", "rife-v3.1"},
        {"RIFE v4", "rife-v4"},
        {"RIFE v4.6", "rife-v4.6"},
        {"RIFE v4.25", "rife-v4.25"},
        {"RIFE v4.25 Lite", "rife-v4.25-lite"},
        {"RIFE v4.26", "rife-v4.26"},
    };

    if (rifeDisplayToFileNameMap.contains(displayName)) {
        return rifeDisplayToFileNameMap.value(displayName);
    }
    return std::nullopt;
}

std::optional<QString> findRifeDisplayNameByModelName(const QString &modelName)
{
    static const QMap<QString, QString> rifeFileNameToDisplayNameMap = {
        {"rife", "RIFE"},
        {"rife-HD", "RIFE HD"},
        {"rife-UHD", "RIFE UHD"},
        {"rife-anime", "RIFE Anime"},
        {"rife-v2", "RIFE v2"},
        {"rife-v2.3", "RIFE v2.3"},
        {"rife-v2.4", "RIFE v2.4"},
        {"rife-v3.0", "RIFE v3.0"},
        {"rife-v3.1", "RIFE v3.1"},
        {"rife-v4", "RIFE v4"},
        {"rife-v4.6", "RIFE v4.6"},
        {"rife-v4.25", "RIFE v4.25"},
        {"rife-v4.25-lite", "RIFE v4.25 Lite"},
        {"rife-v4.26", "RIFE v4.26"},
    };

    if (rifeFileNameToDisplayNameMap.contains(modelName)) {
        return rifeFileNameToDisplayNameMap.value(modelName);
    }
    return std::nullopt;
}

std::optional<QString> generateNewFileName(QString fileName,
                                           video2x::processors::ProcessorType procType,
                                           QString extension)
{
    QFileInfo fileInfo(fileName);
    QString baseFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName() + "."
                           + convertProcessorTypeToLowerCaseQString(procType);
    QString outputFilePath = baseFilePath + extension;
    int counter = 1;

    while (QFileInfo::exists(outputFilePath) && counter > 0) {
        qWarning() << "Warning: file `" << outputFilePath << "` already exists, finding a new name";
        outputFilePath = baseFilePath + "." + QString::number(counter++) + extension;
        qWarning() << "Warning: writing output file to `" << outputFilePath << "`";
    }

    if (!QFileInfo::exists(outputFilePath)) {
        return outputFilePath;
    }
    return std::nullopt;
}

QString getDefaultFontForLocale(const QString &locale)
{
    static const QMap<QString, QString> localeToFontNameMap = {{"zh_CN", "Microsoft YaHei UI"},
                                                               {"ja_JP", "Yu Gothic UI"}};

    if (localeToFontNameMap.contains(locale)) {
        return localeToFontNameMap.value(locale);
    }

    // Use Segoe UI for all other languages
    return "Segoe UI";
}

QString convertProcessorTypeToQString(video2x::processors::ProcessorType procType)
{
    switch (procType) {
    case video2x::processors::ProcessorType::Libplacebo:
        return "libplacebo";
    case video2x::processors::ProcessorType::RealESRGAN:
        return "Real-ESRGAN";
    case video2x::processors::ProcessorType::RealCUGAN:
        return "Real-CUGAN";
    case video2x::processors::ProcessorType::RIFE:
        return "RIFE";
    default:
        return "Unknown";
    }
}

QString convertProcessorTypeToLowerCaseQString(video2x::processors::ProcessorType procType)
{
    switch (procType) {
    case video2x::processors::ProcessorType::Libplacebo:
        return "libplacebo";
    case video2x::processors::ProcessorType::RealESRGAN:
        return "realesrgan";
    case video2x::processors::ProcessorType::RealCUGAN:
        return "realcugan";
    case video2x::processors::ProcessorType::RIFE:
        return "rife";
    default:
        return "unknown";
    }
}

bool isImageFile(const QString &filePath)
{
    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
    return mimeType.name().startsWith("image/");
}
