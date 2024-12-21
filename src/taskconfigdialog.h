#ifndef TASKCONFIGDIALOG_H
#define TASKCONFIGDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

#include "taskconfig.h"

namespace Ui {
class TaskConfigDialog;
}

class TaskConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskConfigDialog(QWidget *parent = nullptr);
    ~TaskConfigDialog();
    void updateScalingFactorAndNoiseLevelRange();
    void execErrorMessage(const QString &message);
    std::optional<TaskConfig> getTaskConfig();
    void setTaskConfig(const TaskConfig &taskConfig);
    void setOutputSuffix(QString suffix);

private slots:
    void on_applyPushButton_clicked();
    void on_addCustomOptionPushButton_clicked();
    void on_removeSelectedOptionPushButton_clicked();
    void on_libplaceboSelectCustomGlslShaderPushButton_clicked();

private:
    void populateVulkanDevices();

    Ui::TaskConfigDialog *ui;
    QStandardItemModel *m_customEncoderOptionsTableModel;
};

#endif // TASKCONFIGDIALOG_H
