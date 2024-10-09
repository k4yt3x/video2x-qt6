#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QModelIndexList>
#include <QStringListModel>
#include <QThread>

extern "C" {
#include "libvideo2x.h"
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
}

#include "./ui_mainwindow.h"
#include "filedroplistview.h"
#include "mainwindow.h"
#include "videoprocessingworker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_procCtx = nullptr;
    m_procStarted = false;
    m_procAborted = false;

    connect(ui->inputSelectionListView,
            &FileDropListView::filesDropped,
            this,
            &MainWindow::handleFilesDropped);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void showErrorMessage(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Error");
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
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

void MainWindow::on_deletedSelectedPushButton_clicked()
{
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());
    if (!model) {
        return;
    }

    // Get the selected indexes
    QModelIndexList selectedIndexes = ui->inputSelectionListView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        return;
    }

    // Iterate over the selected indexes and remove items from the list
    QStringList fileList = model->stringList();
    for (int i = selectedIndexes.size() - 1; i >= 0; --i) {
        fileList.removeAt(selectedIndexes.at(i).row());
    }

    // Update the model with the modified list
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
                ui->startPausePushButton->setText("Resume");
            } else {
                ui->startPausePushButton->setText("Pause");
            }
        }
        return;
    }

    // Access the QListView's model and fetch the video list
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->inputSelectionListView->model());

    if (!model || model->rowCount() == 0) {
        showErrorMessage("No videos in the list to process.");
        qWarning("No videos in the list to process.");
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
    if (currentVideoIndex >= videoList.size()) {
        if (m_procAborted) {
            QMessageBox::warning(this, "Processing aborted", "Video processing aborted!");
        } else {
            QMessageBox::information(this, "Processing complete", "All videos processed.");
        }

        ui->currentProgressBar->setMaximum(1);
        ui->currentProgressBar->setValue(1);
        ui->overallProgressBar->setMaximum(1);
        ui->overallProgressBar->setValue(1);
        ui->startPausePushButton->setText("Start");
        ui->stopPushButton->setEnabled(false);
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
        showErrorMessage("Failed to allocate memory for FilterConfig.");
        return;
    }

    if (ui->filterSelectionComboBox->currentIndex() == 0) {
        // Populate RealESRGAN filter config
        filter_config->filter_type = FILTER_REALESRGAN;
        filter_config->config.realesrgan.gpuid = ui->realesrganGpuIdSpinBox->value();
        filter_config->config.realesrgan.tta_mode = 0;
        filter_config->config.realesrgan.scaling_factor = ui->realesrganScalingFactorSpinBox->value();

        // Convert QString to UTF-8 for the model and store it
        QByteArray model_byte_array = ui->realesrganModelComboBox->currentText().toUtf8();
        filter_config->config.realesrgan.model = strdup(
            model_byte_array.constData()); // strdup for dynamic allocation
    } else if (ui->filterSelectionComboBox->currentIndex() == 1) {
        // Populate libplacebo filter config
        filter_config->filter_type = FILTER_LIBPLACEBO;
        filter_config->config.libplacebo.output_width = ui->libplaceboOutputWidthSpinBox->value();
        filter_config->config.libplacebo.output_height = ui->libplaceboOutputHeightSpinBox->value();

        // Convert QString to UTF-8 for the shader path and store it
        QByteArray shader_byte_array = ui->libplaceboShaderNameLineEdit->text().toUtf8();
        filter_config->config.libplacebo.shader_path = strdup(
            shader_byte_array.constData()); // strdup for dynamic allocation
    } else {
        showErrorMessage("Invalid filter selected!");
        free(filter_config); // Clean up
        return;
    }

    // Parse codec to AVCodec in the main thread before starting worker threads
    const AVCodec *codec = avcodec_find_encoder_by_name(ui->ffmpegCodecLineEdit->text().toUtf8());
    if (!codec) {
        showErrorMessage("Invalid FFmpeg codec.");
        qWarning("Invalid FFmpeg codec.");
        free(filter_config); // Clean up
        return;
    }

    // Parse pixel format to AVPixelFormat in the main thread
    enum AVPixelFormat pix_fmt = av_get_pix_fmt(ui->ffmpegPixFmtLineEdit->text().toUtf8());
    if (pix_fmt == AV_PIX_FMT_NONE) {
        showErrorMessage("Invalid FFmpeg pixel format.");
        qWarning("Invalid FFmpeg pixel format.");
        free(filter_config); // Clean up
        return;
    }

    // Dynamically allocate memory for EncoderConfig and populate it
    EncoderConfig *encoder_config = (EncoderConfig *) malloc(sizeof(EncoderConfig));
    if (!encoder_config) {
        showErrorMessage("Failed to allocate memory for EncoderConfig.");
        free(filter_config); // Clean up
        return;
    }

    // Get hardware device type
    AVHWDeviceType hw_device_type = AV_HWDEVICE_TYPE_NONE;
    if (ui->ffmpegHardwareAccelerationLineEdit->text() != "none") {
        hw_device_type = av_hwdevice_find_type_by_name(
            ui->ffmpegHardwareAccelerationLineEdit->text().toUtf8().constData());
        if (hw_device_type == AV_HWDEVICE_TYPE_NONE) {
            showErrorMessage("Invalid hardware acceleration method.");
            free(filter_config);
            free(encoder_config);
            return;
        }
    }

    // Populate encoder configuration
    QByteArray preset_byte_array = ui->ffmpegPresetLineEdit->text().toUtf8();
    const char *preset_c_string = strdup(preset_byte_array.constData());

    encoder_config->output_width = 0;  // To be filled by libvideo2x
    encoder_config->output_height = 0; // To be filled by libvideo2x
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
    ui->startPausePushButton->setText("Pause");
    ui->stopPushButton->setEnabled(true);
}

void MainWindow::onVideoProcessingFinished(bool retValue, QString inputFilePath)
{
    // Update the overall progress bar
    ui->overallProgressBar->setValue(currentVideoIndex + 1);

    // Check the result of the video processing
    if (retValue) {
        showErrorMessage(QString("Video processing failed for: %1").arg(inputFilePath));
    }

    // Clean up memory for the worker and the thread
    QObject *senderObj = sender(); // Get the sender object (the worker thread)
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
    m_procStarted = false;

    // Move to the next video
    currentVideoIndex++;
    processNextVideo(); // Start processing the next video
}

void MainWindow::on_filterSelectionComboBox_currentIndexChanged(int index)
{
    if (ui->filterSelectionComboBox->currentIndex() == 0) {
        ui->realesrganGroupBox->setEnabled(true);
        ui->libplaceboGroupBox->setEnabled(false);
    } else if (ui->filterSelectionComboBox->currentIndex() == 1) {
        ui->realesrganGroupBox->setEnabled(false);
        ui->libplaceboGroupBox->setEnabled(true);
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
