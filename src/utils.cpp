#include "utils.h"

#include <QFileInfo>
#include <QMap>

std::optional<video2x::logutils::Video2xLogLevel> findLogLevelByName(const QString &logLevelName)
{
    // Static map to store the mapping
    static const std::unordered_map<QString, video2x::logutils::Video2xLogLevel> logLevelMap
        = {{QString("trace"), video2x::logutils::Video2xLogLevel::Trace},
           {QString("debug"), video2x::logutils::Video2xLogLevel::Debug},
           {QString("info"), video2x::logutils::Video2xLogLevel::Info},
           {QString("warning"), video2x::logutils::Video2xLogLevel::Warning},
           {QString("warn"), video2x::logutils::Video2xLogLevel::Warning},
           {QString("error"), video2x::logutils::Video2xLogLevel::Error},
           {QString("critical"), video2x::logutils::Video2xLogLevel::Critical},
           {QString("off"), video2x::logutils::Video2xLogLevel::Off},
           {QString("none"), video2x::logutils::Video2xLogLevel::Off}};

    // Lookup the log level in the map
    auto it = logLevelMap.find(logLevelName);
    if (it != logLevelMap.end()) {
        return it->second;
    }

    return std::nullopt;
}

std::optional<QString> findAnime4kFileNameByDisplayName(const QString &displayName)
{
    static const QMap<QString, QString> anime4kDisplayToFileNameMap
        = {{"Anime4K v4 Mode A", "anime4k-v4-a"},
           {"Anime4K v4 Mode B", "anime4k-v4-b"},
           {"Anime4K v4 Mode C", "anime4k-v4-c"},
           {"Anime4K v4 Mode A+A", "anime4k-v4-a+a"},
           {"Anime4K v4 Mode B+B", "anime4k-v4-b+b"},
           {"Anime4K v4 Mode C+A", "anime4k-v4-c+a"},
           {"Anime4K v4.1 GAN", "anime4k-v4.1-gan"}};

    if (anime4kDisplayToFileNameMap.contains(displayName)) {
        return anime4kDisplayToFileNameMap.value(displayName);
    }
    return std::nullopt;
}

std::optional<QString> findAnime4kDisplayNameByFileName(const QString &fileName)
{
    static const QMap<QString, QString> anime4kFileNameToDisplayNameMap
        = {{"anime4k-v4-a", "Anime4K v4 Mode A"},
           {"anime4k-v4-b", "Anime4K v4 Mode B"},
           {"anime4k-v4-c", "Anime4K v4 Mode C"},
           {"anime4k-v4-a+a", "Anime4K v4 Mode A+A"},
           {"anime4k-v4-b+b", "Anime4K v4 Mode B+B"},
           {"anime4k-v4-c+a", "Anime4K v4 Mode C+A"},
           {"anime4k-v4.1-gan", "Anime4K v4.1 GAN"}};

    if (anime4kFileNameToDisplayNameMap.contains(fileName)) {
        return anime4kFileNameToDisplayNameMap.value(fileName);
    }
    return std::nullopt;
}

std::optional<QString> findRifeModelNameByDisplayName(const QString &displayName)
{
    static const QMap<QString, QString> rifeDisplayToFileNameMap = {{"RIFE", "rife"},
                                                                    {"RIFE HD", "rife-HD"},
                                                                    {"RIFE UHD", "rife-UHD"},
                                                                    {"RIFE Anime", "rife-anime"},
                                                                    {"RIFE v2", "rife-v2"},
                                                                    {"RIFE v2.3", "rife-v2.3"},
                                                                    {"RIFE v2.4", "rife-v2.4"},
                                                                    {"RIFE v3.0", "rife-v3.0"},
                                                                    {"RIFE v3.1", "rife-v3.1"},
                                                                    {"RIFE v4", "rife-v4"},
                                                                    {"RIFE v4.6", "rife-v4.6"}};

    if (rifeDisplayToFileNameMap.contains(displayName)) {
        return rifeDisplayToFileNameMap.value(displayName);
    }
    return std::nullopt;
}

std::optional<QString> findRifeDisplayNameByModelName(const QString &modelName)
{
    static const QMap<QString, QString> rifeFileNameToDisplayNameMap = {{"rife", "RIFE"},
                                                                        {"rife-HD", "RIFE HD"},
                                                                        {"rife-UHD", "RIFE UHD"},
                                                                        {"rife-anime", "RIFE Anime"},
                                                                        {"rife-v2", "RIFE v2"},
                                                                        {"rife-v2.3", "RIFE v2.3"},
                                                                        {"rife-v2.4", "RIFE v2.4"},
                                                                        {"rife-v3.0", "RIFE v3.0"},
                                                                        {"rife-v3.1", "RIFE v3.1"},
                                                                        {"rife-v4", "RIFE v4"},
                                                                        {"rife-v4.6", "RIFE v4.6"}};

    if (rifeFileNameToDisplayNameMap.contains(modelName)) {
        return rifeFileNameToDisplayNameMap.value(modelName);
    }
    return std::nullopt;
}

std::optional<QString> generateNewFileName(QString fileName, QString extension)
{
    QFileInfo fileInfo(fileName);
    QString baseFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".processed";
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

QString convertProcessorTypeToQString(video2x::processors::ProcessorType procType)
{
    switch (procType) {
    case video2x::processors::ProcessorType::Libplacebo:
        return "libplacebo";
    case video2x::processors::ProcessorType::RealESRGAN:
        return "RealESRGAN";
    case video2x::processors::ProcessorType::RIFE:
        return "RIFE";
    default:
        return "Unknown";
    }
}
