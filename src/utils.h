#ifndef UTILS_H
#define UTILS_H

#include <optional>

#include <QProgressBar>
#include <QString>

#include <libvideo2x/libvideo2x.h>

std::optional<Video2xLogLevel> findLogLevelByName(const QString &log_level_name);

std::optional<QString> findAnime4kFileNameByDisplayName(const QString &displayName);

std::optional<QString> findAnime4kDisplayNameByFileName(const QString &fileName);

std::optional<QString> findRifeModelNameByDisplayName(const QString &displayName);

std::optional<QString> findRifeDisplayNameByModelName(const QString &modelName);

std::optional<QString> generateNewFileName(QString fileName, QString extension);

QString convertProcessorTypeToQString(ProcessorType procType);

#endif // UTILS_H
