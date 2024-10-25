#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QMessageBox>
#include <QModelIndexList>
#include <QStringListModel>
#include <QThread>

#ifdef _WIN32
#include <Windows.h>
#endif

extern "C" {
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libvideo2x/libvideo2x.h>
#include <libvideo2x/version.h>
}

#include "filedroplistview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoprocessingworker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_procCtx = nullptr;
    m_procStarted = false;
    m_procAborted = false;

    setWindowTitle("Video2X " + QString::fromUtf8(LIBVIDEO2X_VERSION_STRING));

    connect(ui->inputSelectionListView,
            &FileDropListView::filesDropped,
            this,
            &MainWindow::handleFilesDropped);

    // Set filter setting visibility
    ui->libplaceboGroupBox->setVisible(false);

    // For non-Windows platforms, logs will be shown in the terminal
#ifndef _WIN32
    ui->showLogsCheckBox->setEnabled(false);
#endif

    // Initialize translator
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        if (locale == "en_US") {
            break;
        }
        const QString baseName = "video2x-qt6_" + QLocale(locale).name() + ".qm";
        if (m_translator.load(baseName)) {
            qApp->installTranslator(&m_translator);
            ui->retranslateUi(this);
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showErrorMessage(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

bool MainWindow::changeLanguage(const QString &locale)
{
    if (locale == "en_US") {
        qApp->removeTranslator(&m_translator);
    } else {
        if (m_translator.load("video2x-qt6_" + locale + ".qm")) {
            qApp->removeTranslator(&m_translator);
            qApp->installTranslator(&m_translator);
        } else {
            showErrorMessage("Failed to load translation for locale: " + locale);
            qDebug() << "Failed to load translation for locale:" << locale;
            return false;
        }
    }
    ui->retranslateUi(this);
    qApp->processEvents();
    qApp->setStyle(QApplication::style());
    return true;
}

void MainWindow::on_actionLanguageENUS_triggered()
{
    if (changeLanguage("en_US")) {
        QFont mainWindowFont("Segoe UI");
        this->setFont(mainWindowFont);
    }
}

void MainWindow::on_actionLanguageZHCN_triggered()
{
    if (changeLanguage("zh_CN")) {
        QFont mainWindowFont("Microsoft Yahei");
        this->setFont(mainWindowFont);
    }
}

void MainWindow::on_actionLanguageJAJP_triggered()
{
    if (changeLanguage("ja_JP")) {
        QFont mainWindowFont("Microsoft Yahei");
        this->setFont(mainWindowFont);
    }
}

void MainWindow::on_actionLanguagePTPT_triggered()
{
    if (changeLanguage("pt_PT")) {
        QFont mainWindowFont("Segoe UI");
        this->setFont(mainWindowFont);
    }
}

void MainWindow::handleFilesDropped(const QStringList &fileNames)
{
    // Get the current model
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());
    if (!model) {
        // If there is no model, create one and set it
        model = new QStringListModel(this);
        ui->inputSelectionListView->setModel(model);
    }

    // Get the current list of files
    QStringList currentFiles = model->stringList();

    // Append only the files that are not already in the list
    for (const QString &fileName : fileNames) {
        if (!currentFiles.contains(fileName)) {
            currentFiles.append(fileName);
        }
    }

    // Update the model with the new list
    model->setStringList(currentFiles);
}

void MainWindow::on_addFilesPushButton_clicked()
{
    // Open a file dialog to select one or more files
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Select Files"),
                                                          QString(),
                                                          tr("All Files (*.*)"));

    // No files selected
    if (fileNames.isEmpty()) {
        return;
    }

    // Get the current model
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());
    if (!model) {
        // If there is no model, create one and set it
        model = new QStringListModel(this);
        ui->inputSelectionListView->setModel(model);
    }

    // Get the current list of files
    QStringList currentFiles = model->stringList();

    // Append only the files that are not already in the list
    for (const QString &fileName : fileNames) {
        if (!currentFiles.contains(fileName)) {
            currentFiles.append(fileName);
        }
    }

    // Update the model with the new list
    model->setStringList(currentFiles);
}

void MainWindow::on_deleteSelectedPushButton_clicked()
{
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());
    if (!model) {
        return;
    }

    QModelIndexList selectedIndexes = ui->inputSelectionListView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        return;
    }

    QStringList fileList = model->stringList();

    // Sort the selected indexes in reverse order to prevent index shifting issues
    std::sort(selectedIndexes.rbegin(), selectedIndexes.rend());

    for (const QModelIndex &index : selectedIndexes) {
        fileList.removeAt(index.row());
    }

    model->setStringList(fileList);
}

void MainWindow::on_clearPushButton_clicked()
{
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());
    if (model) {
        model->setStringList(QStringList());
    }
}

void MainWindow::on_startPausePushButton_clicked()
{
    if (m_procStarted) {
        if (m_procCtx != nullptr) {
            m_procCtx->pause = !m_procCtx->pause;
            if (m_procCtx->pause) {
                ui->startPausePushButton->setText(tr("RESUME"));
            } else {
                ui->startPausePushButton->setText(tr("PAUSE"));
            }
        }
        return;
    }

    // Access the QListView's model and fetch the video list
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());

    if (!model || model->rowCount() == 0) {
        showErrorMessage(tr("The job queue is empty!"));
        qWarning("The job queue is empty!");
        return;
    }

    // Get the list of videos to process
    videoList = model->stringList();
    ui->overallProgressBar->setMaximum(videoList.size());
    ui->overallProgressBar->setValue(0);

    currentVideoIndex = 0; // Start from the first video
    m_procStarted = true;
    m_procAborted = false;
    processNextVideo();    // Start processing the first video
}

void MainWindow::processNextVideo()
{
    if (currentVideoIndex >= videoList.size() || m_procAborted) {
        if (m_procAborted) {
            QMessageBox::warning(this, tr("Processing aborted"), tr("Video processing aborted!"));
        } else {
            QMessageBox::information(this, tr("Processing complete"), tr("All videos processed."));
        }
        m_procStarted = false;

        // ui->currentProgressBar->setMaximum(1);
        // ui->currentProgressBar->setValue(1);
        // ui->overallProgressBar->setMaximum(1);
        // ui->overallProgressBar->setValue(1);
        ui->startPausePushButton->setText(tr("START"));
        ui->stopPushButton->setEnabled(false);
        ui->addFilesPushButton->setEnabled(true);
        ui->deleteSelectedPushButton->setEnabled(true);
        ui->clearPushButton->setEnabled(true);
        ui->settingsTabWidget->setEnabled(true);
        ui->menuLanguage->setEnabled(true);
        return;
    }

    // Reset progress bar
    ui->currentProgressBar->setValue(0);

    QString inputFilePath = videoList[currentVideoIndex];

    // Generate output filename alongside the original file directory
    QFileInfo fileInfo(inputFilePath);
    QString outputFilePath = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".processed."
                             + fileInfo.suffix();

    // Dynamically allocate memory for FilterConfig and populate it
    FilterConfig *filter_config = (FilterConfig *) malloc(sizeof(FilterConfig));
    if (!filter_config) {
        showErrorMessage(tr("Failed to allocate memory for FilterConfig."));
        return;
    }

    // Parse log level
    Libvideo2xLogLevel log_level;
    if (ui->debugLogLevelComboBox->currentText() == "trace") {
        log_level = LIBVIDEO2X_LOG_LEVEL_TRACE;
    } else if (ui->debugLogLevelComboBox->currentText() == "debug") {
        log_level = LIBVIDEO2X_LOG_LEVEL_DEBUG;
    } else if (ui->debugLogLevelComboBox->currentText() == "info") {
        log_level = LIBVIDEO2X_LOG_LEVEL_INFO;
    } else if (ui->debugLogLevelComboBox->currentText() == "warning") {
        log_level = LIBVIDEO2X_LOG_LEVEL_WARNING;
    } else if (ui->debugLogLevelComboBox->currentText() == "error") {
        log_level = LIBVIDEO2X_LOG_LEVEL_ERROR;
    } else if (ui->debugLogLevelComboBox->currentText() == "critical") {
        log_level = LIBVIDEO2X_LOG_LEVEL_CRITICAL;
    } else if (ui->debugLogLevelComboBox->currentText() == "off") {
        log_level = LIBVIDEO2X_LOG_LEVEL_OFF;
    } else {
        qWarning() << "Invalid log level specified. Defaulting to 'info'";
        log_level = LIBVIDEO2X_LOG_LEVEL_INFO;
    }

    if (ui->filterSelectionComboBox->currentIndex() == 0) {
        // Populate RealESRGAN filter config
        filter_config->filter_type = FILTER_REALESRGAN;
        filter_config->config.realesrgan.gpuid = ui->realesrganGpuIdSpinBox->value();
        filter_config->config.realesrgan.tta_mode = 0;
        filter_config->config.realesrgan.scaling_factor = ui->realesrganScalingFactorSpinBox->value();

        // Convert QString to UTF-8 for the model and store it
        // TODO: release the memory allocated with strdup
        QByteArray model_byte_array = ui->realesrganModelComboBox->currentText().toUtf8();
        filter_config->config.realesrgan.model = strdup(model_byte_array.constData());
    } else if (ui->filterSelectionComboBox->currentIndex() == 1) {
        // Populate libplacebo filter config
        filter_config->filter_type = FILTER_LIBPLACEBO;
        filter_config->config.libplacebo.out_width = ui->libplaceboOutputWidthSpinBox->value();
        filter_config->config.libplacebo.out_height = ui->libplaceboOutputHeightSpinBox->value();

        // Convert QString to UTF-8 for the shader path and store it
        // TODO: release the memory allocated with strdup
        QByteArray shader_byte_array = ui->libplaceboShaderNameLineEdit->text().toUtf8();
        filter_config->config.libplacebo.shader_path = strdup(shader_byte_array.constData());
    } else {
        showErrorMessage(tr("Invalid filter selected!"));
        free(filter_config); // Clean up
        return;
    }

    // Parse codec to AVCodec in the main thread before starting worker threads
    const AVCodec *codec = avcodec_find_encoder_by_name(ui->ffmpegCodecLineEdit->text().toUtf8());
    if (!codec) {
        showErrorMessage(tr("Invalid FFmpeg video codec."));
        qWarning("Invalid FFmpeg video codec.");
        free(filter_config); // Clean up
        return;
    }

    // Parse pixel format to AVPixelFormat in the main thread
    enum AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
    if (ui->ffmpegPixFmtLineEdit->text().toUtf8() != "auto") {
        pix_fmt = av_get_pix_fmt(ui->ffmpegPixFmtLineEdit->text().toUtf8());
        if (pix_fmt == AV_PIX_FMT_NONE) {
            showErrorMessage(tr("Invalid FFmpeg video pixel format."));
            qWarning("Invalid FFmpeg video pixel format.");
            free(filter_config); // Clean up
            return;
        }
    }

    // Dynamically allocate memory for EncoderConfig and populate it
    EncoderConfig *encoder_config = (EncoderConfig *) malloc(sizeof(EncoderConfig));
    if (!encoder_config) {
        showErrorMessage(tr("Failed to allocate memory for EncoderConfig."));
        free(filter_config); // Clean up
        return;
    }

    // Get hardware device type
    AVHWDeviceType hw_device_type = AV_HWDEVICE_TYPE_NONE;
    if (ui->ffmpegHardwareAccelerationLineEdit->text() != "none") {
        hw_device_type = av_hwdevice_find_type_by_name(
            ui->ffmpegHardwareAccelerationLineEdit->text().toUtf8().constData());
        if (hw_device_type == AV_HWDEVICE_TYPE_NONE) {
            showErrorMessage(tr("Invalid hardware acceleration method."));
            free(filter_config);
            free(encoder_config);
            return;
        }
    }

    // Populate encoder configuration
    // TODO: release the memory allocated with strdup
    QByteArray preset_byte_array = ui->ffmpegPresetLineEdit->text().toUtf8();
    const char *preset_c_string = strdup(preset_byte_array.constData());

    encoder_config->out_width = 0;  // To be filled by libvideo2x
    encoder_config->out_height = 0; // To be filled by libvideo2x
    encoder_config->copy_streams = ui->ffmpegCopyStreamsCheckBox->isChecked();
    encoder_config->codec = codec->id;
    encoder_config->pix_fmt = pix_fmt;
    encoder_config->preset = preset_c_string;
    encoder_config->bit_rate = ui->ffmpegBitRateSpinBox->value();
    encoder_config->crf = static_cast<float>(ui->ffmpegCrfDoubleSpinBox->value());

    // Dynamically allocate VideoProcessingContext on the heap
    VideoProcessingContext *status = (VideoProcessingContext *) malloc(sizeof(VideoProcessingContext));
    if (!status) {
        showErrorMessage("Failed to allocate memory for VideoProcessingContext.");
        free(filter_config);            // Clean up
        free(encoder_config);           // Clean up
        free((void *) preset_c_string); // Free copied string
        return;
    }
    *status = {.processed_frames = 0,
               .total_frames = 0,
               .start_time = time(NULL),
               .pause = false,
               .abort = false,
               .completed = false};
    m_procCtx = status;

    // Create the worker and thread
    VideoProcessingWorker *worker = new VideoProcessingWorker(inputFilePath,
                                                              outputFilePath,
                                                              log_level,
                                                              false,
                                                              hw_device_type,
                                                              filter_config,
                                                              encoder_config,
                                                              status);
    QThread *thread = new QThread;

    // Move the worker to the thread
    worker->moveToThread(thread);

    // Connect signals and slots
    connect(thread, &QThread::started, worker, &VideoProcessingWorker::processVideo);
    connect(worker,
            &VideoProcessingWorker::progressUpdated,
            this,
            [this](int totalFrames, int processedFrames) {
                qDebug() << "Processing: " << processedFrames << "/" << totalFrames << "("
                         << (float) processedFrames / (float) totalFrames << ")";
                ui->currentProgressBar->setMaximum(totalFrames);
                ui->currentProgressBar->setValue(processedFrames);
            });
    connect(worker, &VideoProcessingWorker::finished, this, &MainWindow::onVideoProcessingFinished);

    // Start the thread
    thread->start();

    // Update buttons
    ui->startPausePushButton->setText("PAUSE");
    ui->stopPushButton->setEnabled(true);
    ui->addFilesPushButton->setEnabled(false);
    ui->deleteSelectedPushButton->setEnabled(false);
    ui->clearPushButton->setEnabled(false);
    ui->settingsTabWidget->setEnabled(false);
    ui->menuLanguage->setEnabled(false);
}

void MainWindow::onVideoProcessingFinished(bool retValue, QString inputFilePath)
{
    // Update the overall progress bar
    ui->overallProgressBar->setValue(currentVideoIndex + 1);

    // Check the result of the video processing
    if (retValue) {
        showErrorMessage(QString(tr("Video processing failed for: %1")).arg(inputFilePath));
    }

    // Clean up memory for the worker and the thread
    QObject *senderObj = sender();
    VideoProcessingWorker *worker = qobject_cast<VideoProcessingWorker *>(senderObj);
    if (worker) {
        worker->thread()->quit();
        worker->thread()->wait();
        worker->thread()->deleteLater();
        free(worker->getFilterConfig());
        free(worker->getEncoderConfig());
        free(worker->getStatus());
        worker->deleteLater();
    }
    m_procCtx = nullptr;

    // Move to the next video
    currentVideoIndex++;
    processNextVideo(); // Start processing the next video
}

void MainWindow::on_filterSelectionComboBox_currentIndexChanged(int index)
{
    if (ui->filterSelectionComboBox->currentIndex() == 0) {
        ui->realesrganGroupBox->setEnabled(true);
        ui->libplaceboGroupBox->setEnabled(false);
        ui->realesrganGroupBox->setVisible(true);
        ui->libplaceboGroupBox->setVisible(false);
    } else if (ui->filterSelectionComboBox->currentIndex() == 1) {
        ui->realesrganGroupBox->setEnabled(false);
        ui->libplaceboGroupBox->setEnabled(true);
        ui->realesrganGroupBox->setVisible(false);
        ui->libplaceboGroupBox->setVisible(true);
    }
}

void MainWindow::on_stopPushButton_clicked()
{
    if (m_procStarted) {
        if (m_procCtx != nullptr) {
            m_procCtx->abort = true;
            m_procAborted = true;
        }
        return;
    }
}

void MainWindow::on_debugShowLogsCheckBox_stateChanged(int isChecked)
{
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    if (isChecked) {
        if (hwnd != nullptr) {
            FreeConsole();
            ShowWindow(hwnd, SW_HIDE);

            fclose(stdout);
            fclose(stderr);
        }
        AllocConsole();

        FILE *f_out;
        freopen_s(&f_out, "CONOUT$", "w", stdout);
        FILE *f_err;
        freopen_s(&f_err, "CONOUT$", "w", stderr);
    } else {
        if (hwnd) {
            FreeConsole();
            ShowWindow(hwnd, SW_HIDE);

            fclose(stdout);
            fclose(stderr);
        }
    }
#endif
}

void MainWindow::on_libplaceboSelectGlslShaderPushButton_clicked()
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

    ui->libplaceboShaderNameLineEdit->setText(fileName);
}
