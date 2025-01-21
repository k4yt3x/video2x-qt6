#include "taskconfigdialog.h"
#include "ui_taskconfigdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

extern "C" {
#include <libavutil/hwcontext.h>
}

#include <libvideo2x/logger_manager.h>
#include <vulkan/vulkan.h>

#include "utils.h"

// Processing mode selection
#define FILTER_MODE 0
#define INTERP_MODE 1

// Filter selection
#define PLACEBO_MODE 0
#define ESRGAN_MODE 1
#define CUGAN_MODE 2

// Interpolation selection
#define RIFE_MODE 0

// Real-ESRGAN models
#define REALESRGAN_MODEL_REALESRGAN_PLUS 0
#define REALESRGAN_MODEL_REALESRGAN_PLUS_ANIME 1
#define REALESRGAN_MODEL_REALESR_ANIMEVIDEOV3 2

// Real-CUGAN models
#define REALCUGAN_MODEL_NOSE 0
#define REALCUGAN_MODEL_PRO 1
#define REALCUGAN_MODEL_SE 2

TaskConfigDialog::TaskConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskConfigDialog)
{
    ui->setupUi(this);

    // Setup the model and headers for the custom options table
    m_customEncoderOptionsTableModel = new QStandardItemModel(this);
    m_customEncoderOptionsTableModel->setHorizontalHeaderLabels(QStringList()
                                                                << tr("Option") << tr("Value"));
    ui->customEncoderOptionsTableView->setModel(m_customEncoderOptionsTableModel);

    // Add preset custom option crf=20
    QStandardItem *optionItem = new QStandardItem("crf");
    QStandardItem *valueItem = new QStandardItem("20");
    m_customEncoderOptionsTableModel->appendRow(QList<QStandardItem *>() << optionItem << valueItem);

    // Add preset custom option preset=slow
    optionItem = new QStandardItem("preset");
    valueItem = new QStandardItem("slow");
    m_customEncoderOptionsTableModel->appendRow(QList<QStandardItem *>() << optionItem << valueItem);

    // Set components default visibility
    ui->interpolatorSelectionGroupBox->setVisible(false);
    ui->frameInterpolationOptionsGroupBox->setVisible(false);
    ui->libplaceboOptionsGroupBox->setVisible(false);
    ui->realcuganOptionsGroupBox->setVisible(false);
    ui->rifeOptionsGroupBox->setVisible(false);

    ui->outputWidthLabel->setVisible(false);
    ui->outputHeightLabel->setVisible(false);
    ui->outputWidthSpinBox->setVisible(false);
    ui->outputHeightSpinBox->setVisible(false);

    ui->noiseLevelLabel->setVisible(false);
    ui->noiseLevelSpinBox->setVisible(false);

    // Make processing mode updates also trigger filter/interpolator selection updates
    connect(ui->processingModeSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int) {
                emit ui->filterSelectionComboBox->currentIndexChanged(
                    ui->filterSelectionComboBox->currentIndex());
            });
    connect(ui->processingModeSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int) {
                emit ui->interpolationSelectionComboBox->currentIndexChanged(
                    ui->interpolationSelectionComboBox->currentIndex());
            });

    // Filter options
    connect(ui->processingModeSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) { ui->filterSelectionGroupBox->setVisible(index == FILTER_MODE); });
    connect(ui->processingModeSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) { ui->filterOptionsGroupBox->setVisible(index == FILTER_MODE); });

    connect(ui->filterSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                // Output width and height
                ui->outputWidthLabel->setVisible(index == PLACEBO_MODE);
                ui->outputWidthSpinBox->setVisible(index == PLACEBO_MODE);
                ui->outputHeightLabel->setVisible(index == PLACEBO_MODE);
                ui->outputHeightSpinBox->setVisible(index == PLACEBO_MODE);

                // Scaling factor
                ui->scalingFactorLabel->setVisible(index == ESRGAN_MODE || index == CUGAN_MODE);
                ui->scalingFactorSpinBox->setVisible(index == ESRGAN_MODE || index == CUGAN_MODE);

                // Noise level
                ui->noiseLevelLabel->setVisible(index == CUGAN_MODE);
                ui->noiseLevelSpinBox->setVisible(index == CUGAN_MODE);
            });

    connect(ui->filterSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &TaskConfigDialog::updateScalingFactorAndNoiseLevelRange);

    // Frame interpolation options
    connect(ui->processingModeSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                ui->frameInterpolationOptionsGroupBox->setVisible(index == INTERP_MODE);
            });
    connect(ui->processingModeSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                ui->interpolatorSelectionGroupBox->setVisible(index == INTERP_MODE);
            });

    // Frame rate multiplier
    connect(ui->interpolationSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) { ui->frameRateMultiplierLabel->setVisible(index == RIFE_MODE); });
    connect(ui->interpolationSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) { ui->frameRateMultiplierSpinBox->setVisible(index == RIFE_MODE); });

    // Filter selection
    connect(ui->filterSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                // libplacebo options
                ui->libplaceboOptionsGroupBox->setVisible(
                    ui->processingModeSelectionComboBox->currentIndex() == FILTER_MODE
                    && index == PLACEBO_MODE);

                // Real-ESRGAN options
                ui->realesrganOptionsGroupBox->setVisible(
                    ui->processingModeSelectionComboBox->currentIndex() == FILTER_MODE
                    && index == ESRGAN_MODE);

                // Real-CUGAN options
                ui->realcuganOptionsGroupBox->setVisible(
                    ui->processingModeSelectionComboBox->currentIndex() == FILTER_MODE
                    && index == CUGAN_MODE);
            });

    // Real-ESRGAN options
    connect(ui->realesrganModelComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &TaskConfigDialog::updateScalingFactorAndNoiseLevelRange);

    // Real-CUGAN options
    connect(ui->realcuganModelComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &TaskConfigDialog::updateScalingFactorAndNoiseLevelRange);

    // Interpolation selection
    connect(ui->interpolationSelectionComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                ui->rifeOptionsGroupBox->setVisible(
                    ui->processingModeSelectionComboBox->currentIndex() == INTERP_MODE
                    && index == RIFE_MODE);
            });

    // Cancel button
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);

    // Enumerate and populate Vulkan devices
    populateVulkanDevices();

    // Set default optimal height
    resize(385, 560);
}

TaskConfigDialog::~TaskConfigDialog()
{
    delete ui;
}

void TaskConfigDialog::execErrorMessage(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void TaskConfigDialog::updateScalingFactorAndNoiseLevelRange()
{
    if (ui->filterSelectionComboBox->currentIndex() == ESRGAN_MODE) {
        // For Real-ESRGAN, the maximum scaling factor is 4
        ui->scalingFactorSpinBox->setMaximum(4);

        // Set minimum scaling factor for Real-ESRGAN
        if (ui->realesrganModelComboBox->currentIndex() == REALESRGAN_MODEL_REALESR_ANIMEVIDEOV3
            || ui->realesrganModelComboBox->currentIndex() == REALESRGAN_MODEL_REALESRGAN_PLUS) {
            ui->scalingFactorSpinBox->setMinimum(2);
        } else {
            ui->scalingFactorSpinBox->setMinimum(4);
        }
    } else if (ui->filterSelectionComboBox->currentIndex() == CUGAN_MODE) {
        // For Real-CUGAN, the minimum scaling factor is 2
        ui->scalingFactorSpinBox->setMinimum(2);

        // Set maximum scaling factor and noise level for Real-CUGAN
        switch (ui->realcuganModelComboBox->currentIndex()) {
        case REALCUGAN_MODEL_NOSE:
            ui->scalingFactorSpinBox->setMaximum(2);
            ui->noiseLevelSpinBox->setMinimum(0);
            ui->noiseLevelSpinBox->setMaximum(0);
            break;
        case REALCUGAN_MODEL_PRO:
            ui->scalingFactorSpinBox->setMaximum(3);
            ui->noiseLevelSpinBox->setMinimum(-1);
            ui->noiseLevelSpinBox->setMaximum(3);
            break;
        case REALCUGAN_MODEL_SE:
        default:
            ui->scalingFactorSpinBox->setMaximum(4);
            ui->noiseLevelSpinBox->setMinimum(-1);
            ui->noiseLevelSpinBox->setMaximum(3);
            break;
        }
    } else {
        ui->scalingFactorSpinBox->setMinimum(1);
        ui->scalingFactorSpinBox->setMaximum(99);
    }

    // Clamp scaling factor within the allowed range
    if (ui->scalingFactorSpinBox->value() > ui->scalingFactorSpinBox->maximum()) {
        ui->scalingFactorSpinBox->setValue(ui->scalingFactorSpinBox->maximum());
    } else if (ui->scalingFactorSpinBox->value() < ui->scalingFactorSpinBox->minimum()) {
        ui->scalingFactorSpinBox->setValue(ui->scalingFactorSpinBox->minimum());
    }

    // Clamp noise level within the allowed range
    if (ui->noiseLevelSpinBox->value() > ui->noiseLevelSpinBox->maximum()
        || ui->noiseLevelSpinBox->value() < ui->noiseLevelSpinBox->minimum()) {
        ui->noiseLevelSpinBox->setValue(0);
    }
}

void TaskConfigDialog::populateVulkanDevices()
{
    // Set default placeholder text
    ui->vulkanDeviceSelectionComboBox->addItem(tr("No GPUs with Vulkan support found!"));

    // Create a Vulkan instance
    VkInstance instance;
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        video2x::logger()->error("Failed to create Vulkan instance.");
        return;
    }

    // Enumerate physical devices
    uint32_t device_count = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (result != VK_SUCCESS) {
        video2x::logger()->error("Failed to enumerate Vulkan physical devices.");
        vkDestroyInstance(instance, nullptr);
        return;
    }

    // Check if any devices are found
    if (device_count == 0) {
        video2x::logger()->warn("No Vulkan-compatible physical devices found.");
        vkDestroyInstance(instance, nullptr);
        return;
    }

    // Get physical device properties
    std::vector<VkPhysicalDevice> physical_devices(device_count);
    result = vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data());
    if (result != VK_SUCCESS) {
        video2x::logger()->error("Failed to enumerate Vulkan physical devices.");
        vkDestroyInstance(instance, nullptr);
        return;
    }

    // Populate GPU information
    ui->vulkanDeviceSelectionComboBox->clear();
    for (uint32_t i = 0; i < device_count; i++) {
        VkPhysicalDevice device = physical_devices[i];
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        ui->vulkanDeviceSelectionComboBox->addItem(QString::number(i) + ". "
                                                   + device_properties.deviceName);
        video2x::logger()->debug("Found Vulkan device: {}.", device_properties.deviceName);
    }

    // Clean up Vulkan instance
    vkDestroyInstance(instance, nullptr);
}

std::optional<TaskConfig> TaskConfigDialog::getTaskConfig()
{
    // Create TaskConfig struct on stack
    TaskConfig taskConfig;

    taskConfig.vkDeviceIndex = ui->vulkanDeviceSelectionComboBox->currentIndex();

    // Processing mode selection
    video2x::processors::ProcessingMode procMode;
    switch (ui->processingModeSelectionComboBox->currentIndex()) {
    case 0:
        procMode = video2x::processors::ProcessingMode::Filter;
        break;
    case 1:
        procMode = video2x::processors::ProcessingMode::Interpolate;
        break;
    default:
        video2x::logger()->error("Unknown processing mode selected: {}.",
            ui->processingModeSelectionComboBox->currentIndex());
        return std::nullopt;
    }

    // Processor type
    if (procMode == video2x::processors::ProcessingMode::Filter) {
        switch (ui->filterSelectionComboBox->currentIndex()) {
        case PLACEBO_MODE: { // libplacebo
            taskConfig.procCfg.processor_type = video2x::processors::ProcessorType::Libplacebo;
            taskConfig.procCfg.width = ui->outputWidthSpinBox->value();
            taskConfig.procCfg.height = ui->outputHeightSpinBox->value();
            video2x::processors::LibplaceboConfig libplaceboConfig;
            if (ui->libplaceboCustomGlslShaderPathLineEdit->text().isEmpty()) {
                std::optional<QString> anime4kShaderNameOpt = findAnime4kFileNameByDisplayName(
                    ui->libplaceboGlslShaderComboBox->currentText());

                // This parse should always succeed and return a value
                // If it does not, the text in the UI or the parse is wrong
                if (!anime4kShaderNameOpt.has_value()) {
                    video2x::logger()->error("Failed to parse Anime4K shader name.");
                    return std::nullopt;
                }

                QString anime4kShaderName = anime4kShaderNameOpt.value();
#ifdef _WIN32
                libplaceboConfig.shader_path = anime4kShaderName.toStdWString();
#else
                libplaceboConfig.shader_path = anime4kShaderName.toStdString();
#endif
            } else {
#ifdef _WIN32
                libplaceboConfig.shader_path = ui->libplaceboCustomGlslShaderPathLineEdit->text()
                                                   .toStdWString();
#else
                libplaceboConfig.shader_path = ui->libplaceboCustomGlslShaderPathLineEdit->text()
                                                   .toStdString();
#endif
            }
            taskConfig.procCfg.config = libplaceboConfig;
            break;
        }
        case ESRGAN_MODE: { // Real-ESRGAN
            taskConfig.procCfg.processor_type = video2x::processors::ProcessorType::RealESRGAN;
            taskConfig.procCfg.scaling_factor = ui->scalingFactorSpinBox->value();
            video2x::processors::RealESRGANConfig realesrgan_config;
#ifdef _WIN32
            realesrgan_config.model_name = ui->realesrganModelComboBox->currentText().toStdWString();
#else
            realesrgan_config.model_name = ui->realesrganModelComboBox->currentText().toStdString();
#endif
            taskConfig.procCfg.config = realesrgan_config;
            break;
        }
        case CUGAN_MODE: { // Real-CUGAN
            taskConfig.procCfg.processor_type = video2x::processors::ProcessorType::RealCUGAN;
            taskConfig.procCfg.scaling_factor = ui->scalingFactorSpinBox->value();
            taskConfig.procCfg.noise_level = ui->noiseLevelSpinBox->value();
            video2x::processors::RealCUGANConfig realcugan_config;
#ifdef _WIN32
            realcugan_config.model_name = ui->realcuganModelComboBox->currentText().toStdWString();
#else
            realcugan_config.model_name = ui->realcuganModelComboBox->currentText().toStdString();
#endif
            realcugan_config.num_threads = ui->realcuganThreadsSpinBox->value();
            realcugan_config.syncgap = ui->realcuganSyncGapSpinBox->value();
            taskConfig.procCfg.config = realcugan_config;
            break;
        }
        default:
            video2x::logger()->error("Unknown filter type: {}.", ui->filterSelectionComboBox->currentIndex());
            return std::nullopt;
        }
    } else if (procMode == video2x::processors::ProcessingMode::Interpolate) {
        switch (ui->interpolationSelectionComboBox->currentIndex()) {
        case 0: {
            taskConfig.procCfg.processor_type = video2x::processors::ProcessorType::RIFE;
            taskConfig.procCfg.frm_rate_mul = ui->frameRateMultiplierSpinBox->value();
            taskConfig.procCfg.scn_det_thresh = ui->sceneDetectionThresholdDoubleSpinBox->value();
            video2x::processors::RIFEConfig rifeConfig;

            std::optional<QString> rifeModelNameOpt = findRifeModelNameByDisplayName(
                ui->rifeModelComboBox->currentText());

            // This parse should always succeed and return a value
            // If it does not, the text in the UI or the parse is wrong
            if (!rifeModelNameOpt.has_value()) {
                video2x::logger()->error("Failed to parse RIFE model name.");
                return std::nullopt;
            }

            QString rifeModelName = rifeModelNameOpt.value();
#ifdef _WIN32
            rifeConfig.model_name = rifeModelName.toStdWString();
#else
            rifeConfig.model_name = rifeModelName.toStdString();
#endif
            taskConfig.procCfg.config = rifeConfig;
            break;
        }
        default:
            video2x::logger()->error("Unknown interpolator type: {}", 
                ui->interpolationSelectionComboBox->currentIndex());
            return std::nullopt;
        }
    }

    // EncoderConfig
    taskConfig.outputSuffix = ui->suffixLineEdit->text();
    taskConfig.encCfg.copy_streams = ui->copyStreamsCheckBox->isChecked();

    // Rate control and compression
    taskConfig.encCfg.bit_rate = ui->bitRateSpinBox->value();
    taskConfig.encCfg.rc_buffer_size = ui->rcBufferSizeSpinBox->value();
    taskConfig.encCfg.rc_min_rate = ui->rcMinRateSpinBox->value();
    taskConfig.encCfg.rc_max_rate = ui->rcMaxRateSpinBox->value();
    taskConfig.encCfg.qmin = ui->qMinSpinBox->value();
    taskConfig.encCfg.qmax = ui->qMaxSpinBox->value();

    // GOP and frame structure
    taskConfig.encCfg.gop_size = ui->gopSizeSpinBox->value();
    taskConfig.encCfg.max_b_frames = ui->maxBFramesSpinBox->value();
    taskConfig.encCfg.keyint_min = ui->keyIntMinSpinBox->value();
    taskConfig.encCfg.refs = ui->refsSpinBox->value();

    // Performance and threading
    taskConfig.encCfg.thread_count = ui->threadCountSpinBox->value();

    // Latency and buffering
    taskConfig.encCfg.delay = ui->delaySpinBox->value();

    // Parse codec
    taskConfig.encCfg.codec = "libx264";
    if (avcodec_find_encoder_by_name(ui->codecLineEdit->text().toUtf8().constData()) == nullptr) {
        video2x::logger()->error("Invalid codec specified: '{}'.", ui->codecLineEdit->text().toStdString());
        return std::nullopt;
    }
    taskConfig.encCfg.codec = ui->codecLineEdit->text().toStdString();

    // Parse pix_fmt
    taskConfig.encCfg.pix_fmt = AV_PIX_FMT_NONE;
    if (ui->pixFmtLineEdit->text() != "auto") {
        taskConfig.encCfg.pix_fmt = av_get_pix_fmt(ui->pixFmtLineEdit->text().toUtf8().constData());
        if (taskConfig.encCfg.pix_fmt == AV_PIX_FMT_NONE) {
            video2x::logger()->error("Invalid pix_fmt value: '{}'", ui->pixFmtLineEdit->text().toStdString());
            return std::nullopt;
        }
    }

    // Parse hwaccel
    taskConfig.hwDeviceType = AV_HWDEVICE_TYPE_NONE;
    if (ui->hwaccelLineEdit->text() != "none") {
        taskConfig.hwDeviceType = av_hwdevice_find_type_by_name(
            ui->hwaccelLineEdit->text().toUtf8().constData());
        if (taskConfig.hwDeviceType == AV_HWDEVICE_TYPE_NONE) {
            video2x::logger()->error("Invalid hwaccel value: '{}'", ui->hwaccelLineEdit->text().toStdString());
            return std::nullopt;
        }
    }

    // Populate custom options
    for (int row = 0; row < m_customEncoderOptionsTableModel->rowCount(); ++row) {
        QStandardItem *optionItem = m_customEncoderOptionsTableModel->item(row, 0);
        QStandardItem *valueItem = m_customEncoderOptionsTableModel->item(row, 1);

        // Ensure we have valid items
        if (!optionItem || !valueItem) {
            continue;
        }

        std::string key = optionItem->text().toStdString();
        std::string value = valueItem->text().toStdString();

        if (!key.empty() && !value.empty()) {
            taskConfig.encCfg.extra_opts.emplace_back(key, value);
        }
    }

    return taskConfig;
}

void TaskConfigDialog::setTaskConfig(const TaskConfig &taskConfig)
{
    // vkDeviceIndex
    ui->vulkanDeviceSelectionComboBox->setCurrentIndex(taskConfig.vkDeviceIndex);

    // Determine ProcessingMode by processor_type
    video2x::processors::ProcessingMode procMode;
    switch (taskConfig.procCfg.processor_type) {
    case video2x::processors::ProcessorType::Libplacebo:
    case video2x::processors::ProcessorType::RealESRGAN:
    case video2x::processors::ProcessorType::RealCUGAN:
        procMode = video2x::processors::ProcessingMode::Filter;
        ui->processingModeSelectionComboBox->setCurrentIndex(0); // Filter mode
        break;

    case video2x::processors::ProcessorType::RIFE:
        procMode = video2x::processors::ProcessingMode::Interpolate;
        ui->processingModeSelectionComboBox->setCurrentIndex(1); // Interpolate mode
        break;

    default:
        // Unknown processor type, handle gracefully
        video2x::logger()->error("Unknown processor type in taskConfig");
        return;
    }

    // Set UI based on ProcessingMode and processor_type
    if (procMode == video2x::processors::ProcessingMode::Filter) {
        // Filter Mode
        switch (taskConfig.procCfg.processor_type) {
        case video2x::processors::ProcessorType::Libplacebo: {
            ui->filterSelectionComboBox->setCurrentIndex(0);
            ui->outputWidthSpinBox->setValue(taskConfig.procCfg.width);
            ui->outputHeightSpinBox->setValue(taskConfig.procCfg.height);

            video2x::processors::LibplaceboConfig libplaceboConfig
                = std::get<video2x::processors::LibplaceboConfig>(taskConfig.procCfg.config);
#ifdef _WIN32
            QString shaderName = QString::fromWCharArray(libplaceboConfig.shader_path.c_str());
#else
            QString shaderName = QString::fromStdString(libplaceboConfig.shader_path);
#endif

            if (!shaderName.isEmpty()) {
                std::optional<QString> displayNameOpt = findAnime4kDisplayNameByFileName(shaderName);
                if (displayNameOpt.has_value()) {
                    ui->libplaceboCustomGlslShaderPathLineEdit->clear();
                    ui->libplaceboGlslShaderComboBox->setCurrentText(displayNameOpt.value());
                } else {
                    // Custom shader path
                    ui->libplaceboCustomGlslShaderPathLineEdit->setText(shaderName);
                }
            } else {
                // If empty, maybe set to a default state
                ui->libplaceboCustomGlslShaderPathLineEdit->clear();
                ui->libplaceboGlslShaderComboBox->setCurrentIndex(0);
            }

            break;
        }
        case video2x::processors::ProcessorType::RealESRGAN: {
            ui->filterSelectionComboBox->setCurrentIndex(1);
            ui->scalingFactorSpinBox->setValue(taskConfig.procCfg.scaling_factor);

            video2x::processors::RealESRGANConfig realesrganConfig
                = std::get<video2x::processors::RealESRGANConfig>(taskConfig.procCfg.config);
#ifdef _WIN32
            QString modelName = QString::fromWCharArray(realesrganConfig.model_name.c_str());
#else
            QString modelName = QString::fromStdString(realesrganConfig.model_name);
#endif
            ui->realesrganModelComboBox->setCurrentText(modelName);
            break;
        }
        case video2x::processors::ProcessorType::RealCUGAN: {
            ui->filterSelectionComboBox->setCurrentIndex(2);
            ui->scalingFactorSpinBox->setValue(taskConfig.procCfg.scaling_factor);
            ui->noiseLevelSpinBox->setValue(taskConfig.procCfg.noise_level);

            video2x::processors::RealCUGANConfig realcuganConfig
                = std::get<video2x::processors::RealCUGANConfig>(taskConfig.procCfg.config);
#ifdef _WIN32
            QString modelName = QString::fromWCharArray(realcuganConfig.model_name.c_str());
#else
            QString modelName = QString::fromStdString(realcuganConfig.model_name);
#endif
            ui->realcuganModelComboBox->setCurrentText(modelName);
            ui->realcuganThreadsSpinBox->setValue(realcuganConfig.num_threads);
            ui->realcuganSyncGapSpinBox->setValue(realcuganConfig.syncgap);
            break;
        }
        default:
            // Unknown filter type, handle gracefully
            video2x::logger()->error("Unknown filter processor type.");
            return;
        }
    } else {
        // Interpolate Mode
        switch (taskConfig.procCfg.processor_type) {
        case video2x::processors::ProcessorType::RIFE: {
            ui->interpolationSelectionComboBox->setCurrentIndex(0);
            ui->frameRateMultiplierSpinBox->setValue(taskConfig.procCfg.frm_rate_mul);
            ui->sceneDetectionThresholdDoubleSpinBox->setValue(taskConfig.procCfg.scn_det_thresh);

            video2x::processors::RIFEConfig rifeConfig = std::get<video2x::processors::RIFEConfig>(
                taskConfig.procCfg.config);
#ifdef _WIN32
            QString rifeModelName = QString::fromWCharArray(rifeConfig.model_name.c_str());
#else
            QString rifeModelName = QString::fromStdString(rifeConfig.model_name);
#endif

            // Map back to display name if possible
            // findRifeDisplayNameByModelName is a hypothetical reverse lookup
            std::optional<QString> rifeDisplayNameOpt = findRifeDisplayNameByModelName(
                rifeModelName);
            if (rifeDisplayNameOpt.has_value()) {
                ui->rifeModelComboBox->setCurrentText(rifeDisplayNameOpt.value());
            } else {
                ui->rifeModelComboBox->setCurrentText(rifeModelName);
            }

            break;
        }
        default:
            video2x::logger()->error("Unknown interpolation processor type.");
            return;
        }
    }

    // Suffix
    ui->suffixLineEdit->setText(taskConfig.outputSuffix);

    // copy_streams
    ui->copyStreamsCheckBox->setChecked(taskConfig.encCfg.copy_streams);

    // frameRateMultiplier (only relevant if Interpolate)
    if (procMode == video2x::processors::ProcessingMode::Interpolate) {
        ui->frameRateMultiplierSpinBox->setValue(taskConfig.procCfg.frm_rate_mul);
    } else {
        ui->frameRateMultiplierSpinBox->setValue(0);
    }

    // Rate control and compression
    ui->bitRateSpinBox->setValue(taskConfig.encCfg.bit_rate);
    ui->rcBufferSizeSpinBox->setValue(taskConfig.encCfg.rc_buffer_size);
    ui->rcMinRateSpinBox->setValue(taskConfig.encCfg.rc_min_rate);
    ui->rcMaxRateSpinBox->setValue(taskConfig.encCfg.rc_max_rate);
    ui->qMinSpinBox->setValue(taskConfig.encCfg.qmin);
    ui->qMaxSpinBox->setValue(taskConfig.encCfg.qmax);

    // GOP and frame structure
    ui->gopSizeSpinBox->setValue(taskConfig.encCfg.gop_size);
    ui->maxBFramesSpinBox->setValue(taskConfig.encCfg.max_b_frames);
    ui->keyIntMinSpinBox->setValue(taskConfig.encCfg.keyint_min);
    ui->refsSpinBox->setValue(taskConfig.encCfg.refs);

    // Performance and threading
    ui->threadCountSpinBox->setValue(taskConfig.encCfg.thread_count);

    // Latency and buffering
    ui->delaySpinBox->setValue(taskConfig.encCfg.delay);

    // Codec
    ui->codecLineEdit->setText(QString::fromStdString(taskConfig.encCfg.codec));

    // Pix_fmt
    if (taskConfig.encCfg.pix_fmt == AV_PIX_FMT_NONE) {
        ui->pixFmtLineEdit->setText("auto");
    } else {
        const char *pixFmtName = av_get_pix_fmt_name(taskConfig.encCfg.pix_fmt);
        if (pixFmtName) {
            ui->pixFmtLineEdit->setText(QString::fromUtf8(pixFmtName));
        } else {
            ui->pixFmtLineEdit->setText("auto");
        }
    }

    // hwaccel
    if (taskConfig.hwDeviceType == AV_HWDEVICE_TYPE_NONE) {
        ui->hwaccelLineEdit->setText("none");
    } else {
        const char *hwTypeName = av_hwdevice_get_type_name(taskConfig.hwDeviceType);
        if (hwTypeName) {
            ui->hwaccelLineEdit->setText(QString::fromUtf8(hwTypeName));
        } else {
            ui->hwaccelLineEdit->setText("none");
        }
    }

    // Clear the table first if needed
    m_customEncoderOptionsTableModel->clear();

    // Populate from extra_opts
    for (const auto &kv : taskConfig.encCfg.extra_opts) {
        QString qKey = QString::fromStdString(kv.first);
        QString qValue = QString::fromStdString(kv.second);
        QStandardItem *keyItem = new QStandardItem(qKey);
        QStandardItem *valueItem = new QStandardItem(qValue);
        m_customEncoderOptionsTableModel->appendRow({keyItem, valueItem});
    }
}

void TaskConfigDialog::setOutputSuffix(QString suffix)
{
    ui->suffixLineEdit->setText(suffix);
}

void TaskConfigDialog::on_applyPushButton_clicked()
{
    // Validate Real-ESRGAN scaling factor
    if (ui->processingModeSelectionComboBox->currentIndex() == FILTER_MODE
        && ui->filterSelectionComboBox->currentIndex() == ESRGAN_MODE
        && ui->realesrganModelComboBox->currentIndex() == REALESRGAN_MODEL_REALESRGAN_PLUS
        && ui->scalingFactorSpinBox->value() == 3) {
        execErrorMessage(
            tr("Invalid scaling factor for Real-ESRGAN model 'realesrgan-plus'.\nAllowed values "
               "are 2 and 4"));
        return;
    }

    // Validate Real-CUGAN noise level for models-pro
    if (ui->processingModeSelectionComboBox->currentIndex() == FILTER_MODE
        && ui->filterSelectionComboBox->currentIndex() == CUGAN_MODE
        && ui->realcuganModelComboBox->currentIndex() == REALCUGAN_MODEL_PRO
        && ui->noiseLevelSpinBox->value() > 0 && ui->noiseLevelSpinBox->value() < 3) {
        execErrorMessage(
            tr("Invalid noise level for Real-CUGAN model 'models-pro'.\nAllowed values "
               "are -1, 0, and 3"));
        return;
    }

    // Validate codec
    if (avcodec_find_encoder_by_name(ui->codecLineEdit->text().toUtf8().constData()) == nullptr) {
        execErrorMessage(tr("Invalid codec value: '") + ui->codecLineEdit->text() + "'");
        return;
    }

    // Validate pix_fmt
    if (ui->pixFmtLineEdit->text() != "auto"
        && av_get_pix_fmt(ui->pixFmtLineEdit->text().toUtf8().constData()) == AV_PIX_FMT_NONE) {
        execErrorMessage(tr("Invalid pix_fmt value: '") + ui->pixFmtLineEdit->text() + "'");
        return;
    }

    // Validate hwaccel
    if (ui->hwaccelLineEdit->text() != "none"
        && av_hwdevice_find_type_by_name(ui->hwaccelLineEdit->text().toUtf8().constData())
               == AV_HWDEVICE_TYPE_NONE) {
        execErrorMessage(tr("Invalid hwaccel value: '") + ui->hwaccelLineEdit->text() + "'");
        return;
    }

    // Validate libplacebo GLSL shader path
    if (!ui->libplaceboCustomGlslShaderPathLineEdit->text().isEmpty()) {
        std::filesystem::path glslPath(
            ui->libplaceboCustomGlslShaderPathLineEdit->text().toUtf8().constData());
        if (!std::filesystem::exists(glslPath)) {
            execErrorMessage(tr("libplacebo GLSL shader file not found: '")
                             + ui->libplaceboCustomGlslShaderPathLineEdit->text() + "'");
            return;
        }
    }

    accept();
}

void TaskConfigDialog::on_addCustomOptionPushButton_clicked()
{
    QStandardItem *optionItem = new QStandardItem("");
    QStandardItem *valueItem = new QStandardItem("");
    m_customEncoderOptionsTableModel->appendRow(QList<QStandardItem *>() << optionItem << valueItem);
}

void TaskConfigDialog::on_removeSelectedOptionPushButton_clicked()
{
    if (!ui->customEncoderOptionsTableView->selectionModel()) {
        return;
    }

    QModelIndexList selectedRows = ui->customEncoderOptionsTableView->selectionModel()
                                       ->selectedRows();
    if (selectedRows.isEmpty()) {
        return;
    }

    // Sort the selected rows in reverse order
    std::sort(selectedRows.rbegin(), selectedRows.rend());

    // Remove each selected row from the model
    for (const QModelIndex &index : selectedRows) {
        m_customEncoderOptionsTableModel->removeRow(index.row());
    }
}

void TaskConfigDialog::on_libplaceboSelectCustomGlslShaderPushButton_clicked()
{
    // Open a file dialog to select a .glsl file
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select GLSL Shader"),
                                                    QString(),
                                                    tr("GLSL Files (*.glsl);;All Files (*.*)"));

    // No file selected
    if (fileName.isEmpty()) {
        return;
    }

    ui->libplaceboCustomGlslShaderPathLineEdit->setText(fileName);
}
