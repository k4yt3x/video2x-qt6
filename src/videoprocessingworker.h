#ifndef VIDEOPROCESSINGWORKER_H
#define VIDEOPROCESSINGWORKER_H

#include <QObject>
#include <QString>

#include "libvideo2x.h"

class VideoProcessingWorker : public QObject
{
    Q_OBJECT
public:
    VideoProcessingWorker(const QString &inputFilePath,
                          const QString &outputFilePath,
                          bool benchmark,
                          AVHWDeviceType hwDeviceType,
                          FilterConfig *filterConfig,
                          EncoderConfig *encoderConfig,
                          VideoProcessingContext *status);

    // Getter methods for accessing the allocated data
    FilterConfig *getFilterConfig() const { return m_filterConfig; }
    EncoderConfig *getEncoderConfig() const { return m_encoderConfig; }
    VideoProcessingContext *getStatus() const { return m_status; }

signals:
    void progressUpdated(int totalFrames, int processedFrames); // Signal to update progress bar
    void finished(bool success, QString inputFilePath); // Signal emitted when processing finishes

public slots:
    void processVideo(); // Slot to start the video processing

private:
    QString m_inputFilePath;
    QString m_outputFilePath;
    bool m_benchmark;
    AVHWDeviceType m_hwDeviceType;
    FilterConfig *m_filterConfig;
    EncoderConfig *m_encoderConfig;
    VideoProcessingContext *m_status;
};

#endif // VIDEOPROCESSINGWORKER_H
