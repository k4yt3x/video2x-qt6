#ifndef UTILS_H
#define UTILS_H

#include <optional>

#include <QProgressBar>
#include <QString>

#include <libvideo2x/libvideo2x.h>

#ifdef _WIN32
bool isVCRuntimeRequirementMet();
#endif

std::optional<QString> findAnime4kFileNameByDisplayName(const QString &displayName);

std::optional<QString> findAnime4kDisplayNameByFileName(const QString &fileName);

std::optional<QString> findRifeModelNameByDisplayName(const QString &displayName);

std::optional<QString> findRifeDisplayNameByModelName(const QString &modelName);

std::optional<QString> generateNewFileName(QString fileName,
                                           video2x::processors::ProcessorType procType,
                                           QString extension);

QString getDefaultFontForLocale(const QString &locale);

QString convertProcessorTypeToQString(video2x::processors::ProcessorType procType);

QString convertProcessorTypeToLowerCaseQString(video2x::processors::ProcessorType procType);

#endif // UTILS_H
