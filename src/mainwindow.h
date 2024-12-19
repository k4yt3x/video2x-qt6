#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <filesystem>

#include <QMainWindow>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QTranslator>

#include <libvideo2x/libvideo2x.h>

#include "configmanager.h"
#include "taskprocessor.h"
#include "timer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void processingStarted();
    void processingPaused();
    void processingResumed();
    void processingStopped();

private:
    // Message boxes
    void execErrorMessage(const QString &message);
    void execWarningMessage(const QString &message);

    void updateTaskTableHeaders();
    void checkUpdate();
    bool changeLanguage(const QString &locale);
    void handleFilesDropped(const QStringList &fileNames);
    void addFilesWithConfig(const QStringList &fileNames);
    void setDefaultProgressBarStyle(QProgressBar *progressBar);
    void setProgressBarColor(QProgressBar *progressBar, const QString color);
    QProgressBar *getCurrentProgressBar();
    void processNextVideo();

    Ui::MainWindow *ui;

    // Video2X Qt6 Configs
    ConfigManager m_configManager;
    Video2XConfig m_config;

    // Main UI QTranslator
    QTranslator m_translator;

    // TODO: Remove
    QStandardItemModel *m_taskTableModel = nullptr;

    // Video processing runtime states
    int m_currentVideoIndex = 0;
    TaskProcessor *m_currentTaskProcessor = nullptr;
    bool m_procStarted = false;
    bool m_procAborted = false;
    bool m_hasErrors = false;
    Timer m_timer;

private slots:
    // Custom slots
    void on_progressUpdate(int totalFrames, int processedFrames);
    void on_videoProcessingFinished(bool success, std::filesystem::path inputFilePath);

    // File menu
    void on_actionExit_triggered();
    void on_actionRestore_Defaults_triggered();
    void on_actionReport_Bugs_triggered();
    void on_actionAbout_triggered();

    // Check updates
    void on_updateCommandLinkButton_clicked();
    void on_closeUpdatePushButton_clicked();

    // Changing language
    void on_actionLanguageENUS_triggered();
    void on_actionLanguageZHCN_triggered();
    void on_actionLanguageJAJP_triggered();
    void on_actionLanguagePTPT_triggered();

    // Add/remove tasks
    void on_addFilesPushButton_clicked();
    void on_deleteSelectedPushButton_clicked();
    void on_clearPushButton_clicked();

    // Processing control buttons
    void on_startPushButton_clicked();
    void on_pausePushButton_clicked();
    void on_resumePushButton_clicked();
    void on_abortPushButton_clicked();
    void on_logsCheckBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_neverShowUpdatePushButton_clicked();
};
#endif // MAINWINDOW_H
