#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QAbstractButton>
#include <QDialog>

#include "preferencesmanager.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent, PreferencesManager &prefManager);
    ~PreferencesDialog();
    Video2XPreferences getPreferences();
    void setPreferences(Video2XPreferences pref);

private slots:
    void on_resetAllPreferencesPushButton_clicked();

    void on_openConfigurationFileDirectoryPushButton_clicked();

private:
    Ui::PreferencesDialog *ui;
    PreferencesManager &m_prefManager;
};

#endif // PREFERENCESDIALOG_H
