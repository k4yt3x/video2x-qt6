#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QModelIndexList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QTimer>
#include <QUrl>

#ifdef _WIN32
#include <Windows.h>
#endif

extern "C" {
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
}

#include <libvideo2x/version.h>
#include <vulkan/vulkan.h>

#include "aboutdialog.h"
#include "filedroptableview.h"
#include "taskconfigdialog.h"
#include "utils.h"

#define FILE_NAME_COLUMN 0
#define PROCESSOR_COLUMN 1
#define PROGRESS_BAR_COLUMN 2
#define EDIT_BUTTON_COLUMN 3

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Dynamically set the Window title + library version
    setWindowTitle("Video2X Qt6 " + QString::fromUtf8(LIBVIDEO2X_VERSION_STRING));
    ui->statusbar->showMessage(tr("Status: ") + tr("idle"));

    // Set default component visibility
    ui->upgradeCommandLinkButton->setVisible(false);
    ui->closeUpgradePushButton->setVisible(false);
    ui->pausePushButton->setVisible(false);
    ui->resumePushButton->setVisible(false);
    ui->abortPushButton->setVisible(false);

    // Do not show the logs checkbox on Linux/macOS as they will be printed to the terminal
#ifndef _WIN32
    ui->logsCheckBox->setVisible(false);
    ui->logsVerticalLine->setVisible(false);
#endif

    // Setup tasks table model
    m_taskTableModel = new QStandardItemModel(this);
    m_taskTableModel->setHorizontalHeaderLabels(QStringList() << tr("File Name") << tr("Processor")
                                                              << tr("Progress") << tr("Edit"));
    ui->tasksTableView->setModel(m_taskTableModel);

    // Set table column width after initialization completes
    QTimer::singleShot(0, this, [this]() {
        int totalWidth = ui->tasksTableView->viewport()->width();
        double columnWidthPercentages[] = {0.25, 0.2, 0.4, 0.1};
        for (int i = 0; i < m_taskTableModel->columnCount(); ++i) {
            ui->tasksTableView->setColumnWidth(i, totalWidth * columnWidthPercentages[i]);
        }
    });

    // Connect the QTableView filesDropped signal to the handler
    connect(ui->tasksTableView,
            &FileDropTableView::filesDropped,
            this,
            &MainWindow::addFilesWithConfig);

    // Processing started signal handler
    connect(this, &MainWindow::processingStarted, this, [this]() {
        // Initialize task status variables
        m_currentVideoIndex = 0;
        m_procStarted = true;
        m_procAborted = false;
        m_hasErrors = false;

        ui->startPushButton->setVisible(false);
        ui->pausePushButton->setVisible(true);
        ui->abortPushButton->setVisible(true);

        ui->addFilesPushButton->setEnabled(false);
        ui->deleteSelectedPushButton->setEnabled(false);
        ui->clearPushButton->setEnabled(false);
        ui->menuLanguage->setEnabled(false);

        // Disable all edit buttons
        int rowCount = m_taskTableModel->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex editIndex = m_taskTableModel->index(i, EDIT_BUTTON_COLUMN);
            QWidget *widget = ui->tasksTableView->indexWidget(editIndex);
            if (QPushButton *editButton = qobject_cast<QPushButton *>(widget)) {
                editButton->setEnabled(false);
            }
        }
    });

    // Processing paused signal handler
    connect(this, &MainWindow::processingPaused, this, [this]() {
        ui->pausePushButton->setVisible(false);
        ui->resumePushButton->setVisible(true);

        QProgressBar *progressBar = getCurrentProgressBar();
        if (progressBar != nullptr) {
            setProgressBarColor(progressBar, "orange");
            progressBar->setFormat("%v/%m (%p%)");
        }

        ui->statusbar->showMessage(tr("Status: ") + tr("paused"));
    });

    // Processing resumed signal handler
    connect(this, &MainWindow::processingResumed, this, [this]() {
        ui->resumePushButton->setVisible(false);
        ui->pausePushButton->setVisible(true);

        QProgressBar *progressBar = getCurrentProgressBar();
        if (progressBar != nullptr) {
            setProgressBarColor(progressBar, "palette(highlight)");
            progressBar->setFormat("%v/%m (%p%)");
        }

#ifdef _WIN32
        QString fileName = QString::fromStdWString(
            m_currentTaskProcessor->getTaskConfigs().inFname.wstring());
#else
        QString fileName = QString::fromStdString(
            m_currentTaskProcessor->getTaskConfigs().inFname.string());
#endif

        ui->statusbar->showMessage(tr("Status: ") + fileName);
    });

    // Processing stopped signal handler
    connect(this, &MainWindow::processingStopped, this, [this]() {
        if (m_procAborted) {
            QMessageBox::warning(this,
                                 tr("Processing Aborted"),
                                 tr("Video processing was aborted."));
        } else if (m_hasErrors) {
            QMessageBox::warning(this,
                                 tr("Processing Finished"),
                                 tr("Video processing finished with errors."));
        } else {
            QMessageBox::information(this,
                                     tr("Processing Finished"),
                                     tr("All videos have been processed successfully."));
        }

        m_currentTaskProcessor = nullptr;
        m_procStarted = false;
        m_timer.stop();

        ui->pausePushButton->setVisible(false);
        ui->resumePushButton->setVisible(false);
        ui->abortPushButton->setVisible(false);
        ui->startPushButton->setVisible(true);

        ui->addFilesPushButton->setEnabled(true);
        ui->deleteSelectedPushButton->setEnabled(true);
        ui->clearPushButton->setEnabled(true);
        ui->menuLanguage->setEnabled(true);

        // Enable all edit buttons
        int rowCount = m_taskTableModel->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex editIndex = m_taskTableModel->index(i, EDIT_BUTTON_COLUMN);
            QWidget *widget = ui->tasksTableView->indexWidget(editIndex);
            if (QPushButton *editButton = qobject_cast<QPushButton *>(widget)) {
                editButton->setEnabled(true);
            }
        }

        ui->statusbar->showMessage(tr("Status: ") + tr("idle"));
    });

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

    // Check for available upgrades
    checkUpgrade();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionReport_Bugs_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/k4yt3x/video2x/issues/new"));
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.setVersionString("Video2X Qt6 " + QString::fromUtf8(LIBVIDEO2X_VERSION_STRING));
    aboutDialog.exec();
}

void MainWindow::on_upgradeCommandLinkButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/k4yt3x/video2x/releases/latest"));
}

void MainWindow::on_closeUpgradePushButton_clicked()
{
    ui->upgradeCommandLinkButton->setVisible(false);
    ui->closeUpgradePushButton->setVisible(false);
}

void MainWindow::execErrorMessage(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::execWarningMessage(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::checkUpgrade()
{
    // Create a network manager to handle the HTTP request
    auto *networkManager = new QNetworkAccessManager(this);

    // Prepare the API request to get the latest release info from GitHub
    QNetworkRequest apiRequest(QUrl("https://api.github.com/repos/k4yt3x/video2x/releases/latest"));
    apiRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Send the GET request
    QNetworkReply *apiReply = networkManager->get(apiRequest);

    // Handle the API response
    connect(apiReply, &QNetworkReply::finished, this, [this, apiReply]() {
        // Check for network errors
        if (apiReply->error() != QNetworkReply::NoError) {
            qDebug() << "Network error:" << apiReply->errorString();
            apiReply->deleteLater();
            return;
        }

        // Parse the JSON response
        QJsonDocument jsonResponse = QJsonDocument::fromJson(apiReply->readAll());
        if (jsonResponse.isNull() || !jsonResponse.isObject()) {
            qDebug() << "Error parsing JSON response.";
            apiReply->deleteLater();
            return;
        }

        // Extract the tag_name field (latest version) from the JSON response
        QJsonObject responseObject = jsonResponse.object();
        if (!responseObject.contains("tag_name")) {
            qDebug() << "No tag_name field in JSON response.";
            apiReply->deleteLater();
            return;
        }

        QString latestReleaseVersion = responseObject["tag_name"].toString();
        qDebug() << "Latest version:" << latestReleaseVersion;

        // Compare the latest version with the current version
        if (latestReleaseVersion > LIBVIDEO2X_VERSION_STRING) {
            ui->upgradeCommandLinkButton->setVisible(true);
            ui->closeUpgradePushButton->setVisible(true);
            qDebug() << "An upgrade is available.";
        } else {
            qDebug() << "No upgrade available.";
        }

        // Clean up the reply object
        apiReply->deleteLater();
    });
}

void MainWindow::setDefaultProgressBarStyle(QProgressBar *progressBar)
{
    setProgressBarColor(progressBar, "palette(highlight)");
    progressBar->setMinimum(0);
    progressBar->setMaximum(1);
    progressBar->setValue(0);
    progressBar->setFormat(tr("Not yet processed"));
}

void MainWindow::setProgressBarColor(QProgressBar *progressBar, QString backgroundColor)
{
    progressBar->setStyleSheet(QString("QProgressBar {"
                                       "    border: 1px solid palette(shadow);"
                                       "    border-radius: 5px;"
                                       "    text-align: center;"
                                       "    padding: 1px;"
                                       "    background: palette(base);"
                                       "}"
                                       "QProgressBar::chunk {"
                                       "    border-radius: 5px;"
                                       "    margin: 1px;"
                                       "    background-color: %1;"
                                       "}")
                                   .arg(backgroundColor));
}

QProgressBar *MainWindow::getCurrentProgressBar()
{
    // Set the current job's progress bar to 100%
    QModelIndex progressBarIndex = m_taskTableModel->index(m_currentVideoIndex, PROGRESS_BAR_COLUMN);
    QWidget *widget = ui->tasksTableView->indexWidget(progressBarIndex);
    if (widget != nullptr) {
        return qobject_cast<QProgressBar *>(widget);
    }
    return nullptr;
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
            execErrorMessage("Failed to load translation for locale: " + locale);
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
void MainWindow::addFilesWithConfig(const QStringList &fileNames)
{
    if (fileNames.isEmpty()) {
        return;
    }

    // Show the dialog to configure TaskConfig for all files added
    TaskConfigDialog dialog(this);

    // Set the output suffix based on the first input file's suffix
    // QFileInfo fileInfo = QFileInfo(fileNames[0]);
    // dialog.setOutputSuffix("." + fileInfo.suffix());

    // Show the dialog and wait for it to return
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    // Get the TaskConfig struct from the dialog
    std::optional<TaskConfig> taskOpt = dialog.getTaskConfig();
    if (!taskOpt.has_value()) {
        execErrorMessage(tr("An internal error occurred while parsing the task options."));
        return;
    }
    TaskConfig taskConfig = taskOpt.value();

    // Add each file into the table and attach the TaskConfig
    for (const QString &fileName : fileNames) {
        // Check if this file is already in the model
        bool alreadyExists = false;
        for (int row = 0; row < m_taskTableModel->rowCount(); ++row) {
            QStandardItem *existingItem = m_taskTableModel->item(row, 0);
            if (existingItem && existingItem->text() == fileName) {
                alreadyExists = true;
                break;
            }
        }

        if (alreadyExists) {
            continue;
        }

// Set the input file name to the selected file
#ifdef _WIN32
        taskConfig.inFname = std::filesystem::path(fileName.toStdWString());
        QStandardItem *fileNameItem = new QStandardItem(
            QString::fromStdWString(taskConfig.inFname.filename().wstring()));
#else
        taskConfig.inFname = std::filesystem::path(fileName.toStdString());
        QStandardItem *fileNameItem = new QStandardItem(
            QString::fromStdString(taskConfig.inFname.filename().string()));

#endif

        // Create items for each column
        QStandardItem *processorItem = new QStandardItem(
            convertProcessorTypeToQString(taskConfig.procCfg.processor_type));
        QStandardItem *progressItem = new QStandardItem("0%");
        QStandardItem *editItem = new QStandardItem("Edit");

        // Append the row
        m_taskTableModel->appendRow({fileNameItem, processorItem, progressItem, editItem});

        // Store the TaskConfigs object in the first column's item using a custom role
        fileNameItem->setData(QVariant::fromValue(taskConfig), Qt::UserRole + 1);

        // Add progress bar
        QModelIndex progressIndex = m_taskTableModel->index(m_taskTableModel->rowCount() - 1,
                                                            PROGRESS_BAR_COLUMN);
        QProgressBar *progressBar = new QProgressBar(ui->tasksTableView);
        setDefaultProgressBarStyle(progressBar);
        ui->tasksTableView->setIndexWidget(progressIndex, progressBar);

        // Add an Edit button
        QModelIndex editIndex = m_taskTableModel->index(m_taskTableModel->rowCount() - 1,
                                                        EDIT_BUTTON_COLUMN);
        QPushButton *editButton = new QPushButton(tr("Edit"), ui->tasksTableView);
        editButton->setFocusPolicy(Qt::NoFocus);
        ui->tasksTableView->setIndexWidget(editIndex, editButton);

        // Connect the edit button with an edit function
        connect(editButton, &QPushButton::clicked, this, [this, editIndex]() {
            QStandardItem *item = m_taskTableModel->item(editIndex.row(), 0);
            if (item == nullptr) {
                return;
            }
            QVariant var = item->data(Qt::UserRole + 1);
            if (var.isValid()) {
                // Open the dialog again for editing
                TaskConfigDialog editDialog(this);
                editDialog.setWindowTitle(tr("Edit Task"));

                TaskConfig originalTaskConfig = var.value<TaskConfig>();
                editDialog.setTaskConfig(originalTaskConfig);

                // Do not apply anything if the user clickec cancel
                if (editDialog.exec() != QDialog::Accepted) {
                    return;
                }

                // Parse the GUI option into a TaskConfig
                auto updatedTaskOpt = editDialog.getTaskConfig();
                if (!updatedTaskOpt.has_value()) {
                    execErrorMessage(tr("Failed to parse task configs."));
                    return;
                }
                TaskConfig updatedTask = updatedTaskOpt.value();
                updatedTask.inFname = originalTaskConfig.inFname;
                updatedTask.outFname = originalTaskConfig.outFname;

                // Update the processor column
                m_taskTableModel->item(editIndex.row(), PROCESSOR_COLUMN)
                    ->setText(convertProcessorTypeToQString(updatedTask.procCfg.processor_type));

                // Store updated task
                item->setData(QVariant::fromValue(updatedTask), Qt::UserRole + 1);
            }
        });
    }
}

void MainWindow::on_addFilesPushButton_clicked()
{
    // Open a file dialog to select one or more files
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Select Files"),
                                                          QString(),
                                                          tr("All Files (*.*)"));
    addFilesWithConfig(fileNames);
}

void MainWindow::on_deleteSelectedPushButton_clicked()
{
    QModelIndexList selectedIndexes = ui->tasksTableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        return;
    }

    // Sort the selected indexes in reverse order to prevent index shifting issues
    std::sort(selectedIndexes.rbegin(), selectedIndexes.rend());

    for (const QModelIndex &index : selectedIndexes) {
        m_taskTableModel->removeRow(index.row());
    }
}

void MainWindow::on_clearPushButton_clicked()
{
    // Remove all rows from the model
    m_taskTableModel->removeRows(0, m_taskTableModel->rowCount());
}

void MainWindow::on_startPushButton_clicked()
{
    // Check if the task queue is empty
    if (m_taskTableModel->rowCount() == 0) {
        execWarningMessage(tr("The task queue is empty!"));
        qWarning() << "Warning: Processing aborted; task queue empty";
        return;
    }

    // Set the overall progress bar maximum based on the number of tasks
    ui->overallProgressBar->setMaximum(m_taskTableModel->rowCount());
    ui->overallProgressBar->setValue(0);

    emit processingStarted();
    processNextVideo();
}

void MainWindow::on_pausePushButton_clicked()
{
    if (m_currentTaskProcessor != nullptr) {
        video2x::VideoProcessor *videoProcessor = m_currentTaskProcessor->getVideoProcessor();
        {
            if (videoProcessor != nullptr) {
                videoProcessor->pause();
                m_timer.pause();
                emit processingPaused();
            }
        }
    }
}

void MainWindow::on_resumePushButton_clicked()
{
    if (m_currentTaskProcessor != nullptr) {
        video2x::VideoProcessor *videoProcessor = m_currentTaskProcessor->getVideoProcessor();
        {
            if (videoProcessor != nullptr) {
                videoProcessor->resume();
                m_timer.resume();
                emit processingResumed();
            }
        }
    }
}

void MainWindow::on_abortPushButton_clicked()
{
    if (m_procStarted) {
        if (m_currentTaskProcessor != nullptr) {
            video2x::VideoProcessor *videoProcessor = m_currentTaskProcessor->getVideoProcessor();
            {
                if (videoProcessor != nullptr) {
                    videoProcessor->abort();
                    m_procAborted = true;
                    m_timer.pause();
                }
            }
        }
        return;
    }
}

void MainWindow::on_logsCheckBox_checkStateChanged(const Qt::CheckState &checkState)
{
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    if (checkState == Qt::CheckState::Checked) {
        ui->logsCheckBox->setEnabled(false);

        if (hwnd == nullptr) {
            if (!AllocConsole()) {
                execErrorMessage(tr("Failed to allocate the logging console."));
                return;
            }
        }

        // Redirect stdout and stderr
        FILE *f_out = nullptr;
        FILE *f_err = nullptr;
        if (freopen_s(&f_out, "CONOUT$", "w", stdout) != 0) {
            execErrorMessage(tr("Failed to redirect standard output to the logging console."));
            return;
        }
        if (freopen_s(&f_err, "CONOUT$", "w", stderr) != 0) {
            execErrorMessage(tr("Failed to redirect standard error to the logging console."));
            return;
        }
    }
#endif
}

void MainWindow::on_videoProcessingFinished(bool retValue, std::filesystem::path inputFilePath)
{
    // Set the current job's progress bar to 100%
    QProgressBar *progressBar = getCurrentProgressBar();

    if (progressBar != nullptr) {
        if (m_procAborted || retValue != 0) {
            setProgressBarColor(progressBar, "red");
            QString statusMsg = m_procAborted ? tr("Aborted") : tr("Failed");
            if (progressBar->value() == 0 && progressBar->maximum() == 1) {
                progressBar->setMinimum(0);
                progressBar->setMaximum(1);
                progressBar->setValue(1);
                progressBar->setFormat(statusMsg);
            } else {
                progressBar->setFormat(QString("%v/%m (%p%) - %1").arg(statusMsg));
            }
        } else {
            setProgressBarColor(progressBar, "green");
            progressBar->setValue(progressBar->maximum());
            progressBar->setFormat("%v/%m (%p%)");
        }
    }

    // Update the overall progress bar
    ui->overallProgressBar->setValue(m_currentVideoIndex + 1);

    // Check the result of the video processing
    if (retValue && !m_procAborted) {
        m_hasErrors = true;
#ifdef _WIN32
        std::wstring inputFilePathStr = inputFilePath.wstring();
#else
        std::string inputFilePathStr = inputFilePath.string();
#endif
    }

    // Clean up memory for the worker and the thread
    QObject *senderObj = sender();
    TaskProcessor *worker = qobject_cast<TaskProcessor *>(senderObj);
    if (worker) {
        worker->thread()->quit();
        worker->thread()->wait();
        worker->thread()->deleteLater();
        worker->deleteLater();
    }

    // Process Fthe next video
    m_currentVideoIndex++;
    processNextVideo();
}

void MainWindow::on_progressUpdate(int totalFrames, int processedFrames)
{
    qDebug() << "Processing: " << processedFrames << "/" << totalFrames << "("
             << (float) processedFrames / (float) totalFrames << ")";

    // Get the progress bar for the current row
    QProgressBar *progressBar = getCurrentProgressBar();

    // Set progress bar value to the number of processed frames
    if (progressBar != nullptr) {
        progressBar->setMaximum(totalFrames);
        progressBar->setValue(processedFrames);
    }

    // Get elapsed time
    int64_t elapsedMilliseconds = m_timer.getElapsedTime();

    // Calculate average frames per second
    double elapsedSecondsPerFrame = processedFrames
                                    / (static_cast<double>(elapsedMilliseconds) / 1000);
    QString elapsedSecondsString = QString::number(elapsedSecondsPerFrame, 'f', 4);
    ui->framesPerSecondLabel->setText(elapsedSecondsString);

    // Convert to hours, minutes, and seconds
    int hours = elapsedMilliseconds / (1000 * 60 * 60);
    int minutes = (elapsedMilliseconds / (1000 * 60)) % 60;
    int seconds = (elapsedMilliseconds / 1000) % 60;

    // Format the time as HH:mm:ss
    QString elapsedString = QString("%1:%2:%3")
                                .arg(hours, 2, 10, QChar('0'))
                                .arg(minutes, 2, 10, QChar('0'))
                                .arg(seconds, 2, 10, QChar('0'));

    // Set the text for the time elapsed label
    ui->timeElapsedLabel->setText(elapsedString);

    // Calculate average time per frame (in milliseconds)
    double avgTimePerFrame = static_cast<double>(elapsedMilliseconds) / processedFrames;

    // Calculate remaining frames
    int remainingFrames = totalFrames - processedFrames;

    // Estimate remaining time in milliseconds
    double remainingMilliseconds = avgTimePerFrame * remainingFrames;

    // Check if remaining time is greater than one day or negative
    const double millisecondsInADay = 24 * 60 * 60 * 1000;
    QString remainingString;

    if (remainingMilliseconds >= millisecondsInADay) {
        // If remaining time is greater than one day
        int remainingDays = static_cast<int>(remainingMilliseconds / millisecondsInADay);
        if (remainingDays < 0) {
            remainingString = "?";
        } else {
            remainingString = QString::number(remainingDays);
        }
        if (remainingDays == 1) {
            remainingString += tr(" day");
        } else {
            remainingString += tr(" days");
        }
    } else {
        // Convert to hours, minutes, and seconds
        int remainingMillisecondsInt = static_cast<int>(remainingMilliseconds);
        int hoursRemaining = remainingMillisecondsInt / (1000 * 60 * 60);
        int minutesRemaining = (remainingMillisecondsInt / (1000 * 60)) % 60;
        int secondsRemaining = (remainingMillisecondsInt / 1000) % 60;

        // Format the time as HH:mm:ss
        remainingString = QString("%1:%2:%3")
                              .arg(hoursRemaining, 2, 10, QChar('0'))
                              .arg(minutesRemaining, 2, 10, QChar('0'))
                              .arg(secondsRemaining, 2, 10, QChar('0'));
    }

    // Set the text for the time remaining label
    ui->timeRemainingLabel->setText(remainingString);
}

void MainWindow::processNextVideo()
{
    // Check if we have reached the end of the tasks or if the process was aborted
    if (m_procAborted || m_currentVideoIndex >= m_taskTableModel->rowCount()) {
        // Video processing ended
        emit processingStopped();
        return;
    }

    // Get the current task's progress bar
    QProgressBar *progressBar = getCurrentProgressBar();

    // Set the progress bar's style
    if (progressBar != nullptr) {
        progressBar->setValue(0);
        setDefaultProgressBarStyle(progressBar);
        progressBar->setFormat("%v/%m (%p%)");
    }

    // Start timer
    m_timer.start();

    // Retrieve the TaskConfigs from the model
    QStandardItem *item = m_taskTableModel->item(m_currentVideoIndex, 0);
    if (!item) {
        // No valid item means no task or an error—just consider processing complete
        emit processingStopped();
        return;
    }

    QVariant var = item->data(Qt::UserRole + 1);
    if (!var.isValid()) {
        // If no valid TaskConfigs is stored, consider it done
        emit processingStopped();
        return;
    }

    TaskConfig taskConfig = var.value<TaskConfig>();

    // Use the inFname from the task configs
#ifdef _WIN32
    QString inputFileName = QString::fromStdWString(taskConfig.inFname.wstring());
#else
    QString inputFileName = QString::fromStdString(taskConfig.inFname.string());
#endif
    ui->statusbar->showMessage(tr("Status: ") + tr("Processing file ") + inputFileName);

    // Set the outFname
    std::optional<QString> outputFilePath = generateNewFileName(inputFileName,
                                                                taskConfig.outputSuffix);
    if (!outputFilePath.has_value()) {
        execErrorMessage(tr("Failed to generate output file name for file: ") + inputFileName);
        return;
    }

#ifdef _WIN32
    taskConfig.outFname = std::filesystem::path(outputFilePath.value().toStdWString());
#else
    taskConfig.outFname = std::filesystem::path(outputFilePath.value().toStdString());
#endif

    // Create the worker and thread
    TaskProcessor *taskProcessor = new TaskProcessor(taskConfig);
    m_currentTaskProcessor = taskProcessor;
    QThread *workerThread = new QThread;

    // Move the worker to the thread
    taskProcessor->moveToThread(workerThread);

    // Connect signals and slots
    connect(workerThread, &QThread::started, taskProcessor, &TaskProcessor::on_processVideo);
    connect(taskProcessor, &TaskProcessor::on_progressUpdated, this, &MainWindow::on_progressUpdate);
    connect(taskProcessor,
            &TaskProcessor::on_processingFinished,
            this,
            &MainWindow::on_videoProcessingFinished);

    // Start the thread
    workerThread->start();
}
