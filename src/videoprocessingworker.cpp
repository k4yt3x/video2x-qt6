#include "videoprocessingworker.h"
#include <QDebug>
#include <QThread>

// Constructor
VideoProcessingWorker::VideoProcessingWorker(const QString &inputFilePath,
                                             const QString &outputFilePath,
                                             Libvideo2xLogLevel logLevel,
                                             bool benchmark,
                                             AVHWDeviceType hwDeviceType,
                                             FilterConfig *filterConfig,
                                             EncoderConfig *encoderConfig,
                                             VideoProcessingContext *status)
    : m_inputFilePath(inputFilePath)
    , m_outputFilePath(outputFilePath)
    , m_logLevel(logLevel)
    , m_benchmark(benchmark)
    , m_hwDeviceType(hwDeviceType)
    , m_filterConfig(filterConfig)
    , m_encoderConfig(encoderConfig)
    , m_status(status)
{}

// Process Video Slot
void VideoProcessingWorker::processVideo()
{
    // Start a thread to monitor progress while process_video is running
    bool processingFinished = false;

    // Start monitoring the progress in another thread
    QThread *progressThread = QThread::create([this, &processingFinished]() {
        while (!processingFinished) {
            QThread::msleep(100); // Sleep to avoid busy-waiting

            // Ensure that process_video sets total_frames and updates processed_frames
            if (m_status->total_frames > 0) {
                // Emit the progress update signal to the main thread
                emit progressUpdated(m_status->total_frames, m_status->processed_frames);
            }
        }
    });
    progressThread->start();

    // Call the blocking process_video function (pure C function)
    bool success = process_video(m_inputFilePath.toUtf8().constData(),
                                 m_outputFilePath.toUtf8().constData(),
                                 m_logLevel,
                                 m_benchmark,
                                 m_hwDeviceType,
                                 m_filterConfig,
                                 m_encoderConfig,
                                 m_status);

    // Signal that processing has finished, so the progress monitoring loop can stop
    processingFinished = true;
    progressThread->quit();
    progressThread->wait();
    delete progressThread; // Clean up the monitoring thread

    // Emit finished signal to notify that the process has completed
    emit finished(success, m_inputFilePath);
}
