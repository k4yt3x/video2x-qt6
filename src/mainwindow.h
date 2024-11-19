#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>

extern "C" {
#include <libvideo2x/libvideo2x.h>
}

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

private:
    int currentVideoIndex;
    Ui::MainWindow *ui;
    VideoProcessingContext *m_procCtx;
    bool m_procStarted;
    bool m_procAborted;
    QTranslator m_translator;
    Timer m_timer;
    QStringList videoList;

    void populateVulkanDevices();
    void processNextVideo();
    void execErrorMessage(const QString &message);
    void openErrorMessage(const QString &message);
    void execWarningMessage(const QString &message);
    bool changeLanguage(const QString &locale);
    void handleFilesDropped(const QStringList &fileNames);
    void on_VideoProcessingFinished(bool success, QString inputFilePath);
    bool m_losslessEncodingEnabled = false;

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionReport_Bugs_triggered();
    void on_actionLanguageENUS_triggered();
    void on_actionLanguageZHCN_triggered();
    void on_actionLanguageJAJP_triggered();
    void on_actionLanguagePTPT_triggered();
    void on_addFilesPushButton_clicked();
    void on_deleteSelectedPushButton_clicked();
    void on_clearPushButton_clicked();
    void on_startPausePushButton_clicked();
    void on_filterSelectionComboBox_currentIndexChanged(int index);
    void on_stopPushButton_clicked();
    void on_debugShowLogsCheckBox_stateChanged(int arg1);
    void on_libplaceboSelectGlslShaderPushButton_clicked();
    void on_realesrganModelComboBox_currentTextChanged(const QString &arg1);
    void on_losslessCheckBox_stateChanged(int state);
};
#endif // MAINWINDOW_H
