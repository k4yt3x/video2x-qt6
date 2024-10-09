#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern "C" {
#include "libvideo2x.h"
}

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
    QStringList videoList;
    void processNextVideo();

private slots:
    void on_actionExit_triggered();

    void handleFilesDropped(const QStringList &fileNames);

    void on_addFilesPushButton_clicked();

    void on_deletedSelectedPushButton_clicked();

    void on_clearPushButton_clicked();

    void on_startPausePushButton_clicked();

    void onVideoProcessingFinished(bool success, QString inputFilePath);

    void on_filterSelectionComboBox_currentIndexChanged(int index);

    void on_stopPushButton_clicked();

private:
    Ui::MainWindow *ui;
    VideoProcessingContext *m_procCtx;
    bool m_procStarted;
    bool m_procAborted;
};
#endif // MAINWINDOW_H
