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

#ifdef _WIN32
    std::wstring inputFilePathWString = m_inputFilePath.toStdWString();
    const wchar_t *inputFilePath = inputFilePathWString.c_str();

    std::wstring outputFilePathWString = m_outputFilePath.toStdWString();
    const wchar_t *outputFilePath = outputFilePathWString.c_str();
#else
    QByteArray inputFilePathUtf8 = m_inputFilePath.toUtf8();
    const char *inputFilePath = inputFilePathUtf8.constData();

    QByteArray outputFilePathUtf8 = m_outputFilePath.toUtf8();
    const char *outputFilePath = outputFilePathUtf8.constData();
#endif

    // Call the blocking process_video function (pure C function)
    bool success = process_video(inputFilePath,
                                 outputFilePath,
                                 m_logLevel,
                                 m_benchmark,
                                 m_hwDeviceType,
                                 m_filterConfig,
                                 m_encoderConfig,
                                 m_status);

    // Free duplicated strings
    if (m_encoderConfig->preset) {
        free(const_cast<char *>(m_encoderConfig->preset));
    }
    if (m_filterConfig->filter_type == FILTER_LIBPLACEBO) {
#ifdef _WIN32
        free(const_cast<wchar_t *>(m_filterConfig->config.libplacebo.shader_path));
#else
        free(const_cast<char *>(m_filterConfig->config.libplacebo.shader_path));
#endif
    }
    if (m_filterConfig->filter_type == FILTER_REALESRGAN) {
#ifdef _WIN32
        free(const_cast<wchar_t *>(m_filterConfig->config.realesrgan.model_name));
#else
        free(const_cast<char *>(m_filterConfig->config.realesrgan.model_name));
#endif
    }

    // Signal that processing has finished, so the progress monitoring loop can stop
    processingFinished = true;
    progressThread->quit();
    progressThread->wait();
    delete progressThread; // Clean up the monitoring thread

    // Emit finished signal to notify that the process has completed
    emit finished(success, m_inputFilePath);
}
