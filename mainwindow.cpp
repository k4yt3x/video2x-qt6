#include <QMessageBox>
#include <QFileDialog>
#include <QStringListModel>
#include <QModelIndexList>
#include <QDebug>

extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include "libvideo2x.h"
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

void MainWindow::on_addFilesPushButton_clicked()
{
    // Open a file dialog to select one or more files
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Select Files"),
        QString(),
        tr("All Files (*.*)")
    );

    // No files selected
    if (fileNames.isEmpty()) {
        return;
    }

    // If the QListView does not already have a model, we create one
    QStringListModel *model = qobject_cast<QStringListModel*>(ui->inputSelectionListView->model());
    if (!model) {
        // Create a new model if the QListView doesn't have one yet
        model = new QStringListModel(this);
        ui->inputSelectionListView->setModel(model);
    }

    // Append the new files to the model
    QStringList currentFiles = model->stringList();
    currentFiles.append(fileNames);
    model->setStringList(currentFiles);
}

void MainWindow::on_deletedSelectedPushButton_clicked()
{
    QStringListModel *model = qobject_cast<QStringListModel*>(ui->inputSelectionListView->model());
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
    QStringListModel *model = qobject_cast<QStringListModel*>(ui->inputSelectionListView->model());
    if (model) {
        model->setStringList(QStringList());
    }
}

void MainWindow::on_startPausePushButton_clicked()
{
    // Setup filter configurations based on the parsed arguments
    struct FilterConfig filter_config;
    if (ui->filterSelectionComboBox->currentIndex() == 0) {
        filter_config.filter_type = FILTER_REALESRGAN;
        filter_config.config.realesrgan.gpuid = ui->commonGpuIdSpinBox->value();
        filter_config.config.realesrgan.tta_mode = 0;
        filter_config.config.realesrgan.scaling_factor = ui->realesrganScalingFactorSpinBox->value();
        filter_config.config.realesrgan.model = ui->realesrganModelComboBox->currentText().toUtf8();
    } else if (ui->filterSelectionComboBox->currentIndex() == 1) {
        filter_config.filter_type = FILTER_LIBPLACEBO;
        filter_config.config.libplacebo.output_width = ui->libplaceboOutputWidthSpinBox->value();
        filter_config.config.libplacebo.output_height = ui->libplaceboOutputHeightSpinBox->value();
        filter_config.config.libplacebo.shader_path = ui->libplaceboShaderNameLineEdit->text().toUtf8();
    } else {
        showErrorMessage("Invalid filter selected!");
        qWarning("Invalid filter selected!");
        return;
    }

    // Parse codec to AVCodec
    const AVCodec *codec = avcodec_find_encoder_by_name(ui->ffmpegCodecLineEdit->text().toUtf8());
    if (!codec) {
        showErrorMessage("Invalid FFmpeg codec.");
        qWarning("Invalid FFmpeg codec.");
        return;
    }

    // Parse pixel format to AVPixelFormat
    enum AVPixelFormat pix_fmt = av_get_pix_fmt(ui->ffmpegPixFmtLineEdit->text().toUtf8());
    if (pix_fmt == AV_PIX_FMT_NONE) {
        showErrorMessage("Invalid FFmpeg pixel format.");
        qWarning("Invalid FFmpeg pixel format.");
        return;
    }

    QByteArray preset_byte_array = ui->ffmpegPresetLineEdit->text().toUtf8();
    const char* preset_c_string = preset_byte_array.constData();

    // Setup encoder configuration
    struct EncoderConfig encoder_config = {
        .output_width = 0,   // To be filled by libvideo2x
        .output_height = 0,  // To be filled by libvideo2x
        .codec = codec->id,
        .pix_fmt = pix_fmt,
        .preset = preset_c_string,
        .bit_rate = ui->ffmpegBitRateSpinBox->value(),
        .crf = static_cast<float>(ui->ffmpegCrfDoubleSpinBox->value()),
    };

    // Setup struct to store processing status
    struct ProcessingStatus status = {0};

    // Process the video

    // Access the QListView's model
    QStringListModel *model = qobject_cast<QStringListModel*>(ui->inputSelectionListView->model());

    // Check if the model exists and contains any items
    if (!model || model->rowCount() == 0) {
        showErrorMessage("No videos in the list to process.");
        qWarning("No videos in the list to process.");
        return;
    }

    QStringList videoList = model->stringList();
    ui->overallProgressBar->setMaximum(videoList.size());

    // Iterate through each video in the list
    for (const QString &inputFilePath : videoList) {
        // Generate output filename by appending "_processed" before the file extension
        QFileInfo fileInfo(inputFilePath);
        QString outputFilePath = fileInfo.completeBaseName() + "_processed." + fileInfo.suffix();

        // Call the process_video function
        if (process_video(inputFilePath.toUtf8().constData(),
                          outputFilePath.toUtf8().constData(),
                          &filter_config,
                          &encoder_config,
                          &status)) {
            showErrorMessage(QString("Video processing failed for: %1").arg(inputFilePath));
            return;  // Stop processing if an error occurs
        }
        ui->overallProgressBar->setValue(ui->overallProgressBar->value()+1);
    }

    QMessageBox::information(this, "Success", "All videos processed successfully.");
}

