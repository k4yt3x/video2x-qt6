#ifndef TASKPROCESSOR_H
#define TASKPROCESSOR_H

#include <atomic>

#include <QObject>
#include <QString>

#include <libvideo2x/libvideo2x.h>

#include "taskconfig.h"

class TaskProcessor : public QObject
{
    Q_OBJECT
public:
    TaskProcessor(TaskConfig taskConfigs);

    // Getter methods for accessing the allocated data
    const TaskConfig &getTaskConfigs() const { return m_taskConfigs; }

    // Getter for the video processor
    video2x::VideoProcessor *getVideoProcessor() { return m_videoProcessor.load(); }

signals:
    void on_progressUpdated(int totalFrames, int processedFrames, int recoveredFrames);
    void on_processingFinished(bool success, std::filesystem::path inputFilePath);

public slots:
    void on_processVideo();

private:
    TaskConfig m_taskConfigs;
    std::atomic<video2x::VideoProcessor *> m_videoProcessor = nullptr;
};

#endif // TASKPROCESSOR_H
