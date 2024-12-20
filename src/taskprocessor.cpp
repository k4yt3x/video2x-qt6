#include "taskprocessor.h"

#include <QDebug>
#include <QThread>

#include <libvideo2x/logger_manager.h>

#include "taskconfig.h"

// Constructor
TaskProcessor::TaskProcessor(TaskConfig taskConfigs)
    : m_taskConfigs(taskConfigs)
{}

// Process Video Slot
void TaskProcessor::on_processVideo()
{
    // Start a thread to monitor progress while process_video is running
    bool processingFinished = false;

    // Create video processor object
    video2x::VideoProcessor videoProcessor = video2x::VideoProcessor(m_taskConfigs.procCfg,
                                                                     m_taskConfigs.encCfg,
                                                                     m_taskConfigs.vkDeviceIndex,
                                                                     m_taskConfigs.hwDeviceType,
                                                                     m_taskConfigs.benchmark);
    m_videoProcessor = &videoProcessor;

    // Start monitoring the progress in another thread
    QThread *progressUpdaterThread = QThread::create([this, &videoProcessor, &processingFinished]() {
        while (!processingFinished) {
            if (m_videoProcessor != nullptr
                && m_videoProcessor->get_state() != video2x::VideoProcessorState::Paused) {
                // Sleep for 100 ms between updates (10 updates/s)
                QThread::msleep(100);

                // Ensure that process_video sets total_frames and updates processed_frames
                if (videoProcessor.get_total_frames() > 0) {
                    // Emit the progress update signal to the main thread
                    emit on_progressUpdated(videoProcessor.get_total_frames(),
                                            videoProcessor.get_processed_frames());
                }
            }
        }
    });
    progressUpdaterThread->start();

    // Call the blocking process_video function (pure C function)
    bool processorReturn = videoProcessor.process(m_taskConfigs.inFname, m_taskConfigs.outFname);

    // Signal that processing has finished, so the progress monitoring loop can stop
    processingFinished = true;
    progressUpdaterThread->quit();
    progressUpdaterThread->wait();
    delete progressUpdaterThread;
    m_videoProcessor = nullptr;

    // Emit finished signal to notify that the process has completed
    emit on_processingFinished(processorReturn, m_taskConfigs.inFname);
}
