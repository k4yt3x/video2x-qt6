#ifndef TASKCONFIG_H
#define TASKCONFIG_H

#include <QString>

#include <libvideo2x/libvideo2x.h>

struct TaskConfig
{
    // VideoProcessor parameters
    ProcessorConfig procCfg;
    EncoderConfig encCfg;
    uint32_t vkDeviceIndex;
    AVHWDeviceType hwDeviceType;
    Video2xLogLevel logLevel;
    bool benchmark = false;

    // VideoProcessor::process parameters
    std::filesystem::path inFname;
    std::filesystem::path outFname;

    // Additional data for restoring the TaskConfigDialog
    QString outputSuffix;
    QString codecName;
};

#endif // TASKCONFIG_H
