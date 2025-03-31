#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QDesktopServices>
#include <QMessageBox>

#include "utils.h"

PreferencesDialog::PreferencesDialog(QWidget *parent, PreferencesManager &prefManager)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
    , m_prefManager(prefManager)
{
    ui->setupUi(this);

    connect(ui->preferenceCategoryListWidget, &QListWidget::currentRowChanged, [&](int currentRow) {
        ui->preferenceStackedWidget->setCurrentIndex(currentRow);
    });
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

Video2XPreferences PreferencesDialog::getPreferences()
{
    Video2XPreferences pref;

    // Parse display language
    switch (ui->languageComboBox->currentIndex()) {
    case 0:
        pref.translation = "system";
        break;
    case 1:
        pref.translation = "en_US";
        break;
    case 2:
        pref.translation = "zh_CN";
        break;
    case 3:
        pref.translation = "ja_JP";
        break;
    case 4:
        pref.translation = "pt_PT";
        break;
    case 5:
        pref.translation = "fr_FR";
        break;
    case 6:
        pref.translation = "de_DE";
        break;
    case 7:
        pref.translation = "ko_KR";
        break;
    default:
        pref.translation = "system";
        break;
    }

    // Parse check updates
    pref.checkUpdates = ui->checkForUpdatesCheckBox->isChecked();

    // Parse action when done
    switch (ui->whenDoneComboBox->currentIndex()) {
    case 0:
        pref.actionWhenDone = Video2XPreferences::ActionWhenDone::DoNothing;
        break;
    case 1:
        pref.actionWhenDone = Video2XPreferences::ActionWhenDone::Shutdown;
        break;
    case 2: 
        pref.actionWhenDone = Video2XPreferences::ActionWhenDone::Sleep;
        break;
    case 3:
        pref.actionWhenDone = Video2XPreferences::ActionWhenDone::Hibernate;
        break;
    default:   
        pref.actionWhenDone = Video2XPreferences::ActionWhenDone::DoNothing;
        break;
    }

    // Parse auto show stats
    pref.autoShowStats = ui->autoShowStatsCheckBox->isChecked();

    // Parse remove finished tasks
    pref.removeFinishedTasks = ui->removeFinishedTasksCheckBox->isChecked();

    return pref;
}

void PreferencesDialog::setPreferences(Video2XPreferences pref)
{
    // Set display language
    if (pref.translation == "system") {
        ui->languageComboBox->setCurrentIndex(0);
    } else if (pref.translation == "en_US") {
        ui->languageComboBox->setCurrentIndex(1);
    } else if (pref.translation == "zh_CN") {
        ui->languageComboBox->setCurrentIndex(2);
    } else if (pref.translation == "ja_JP") {
        ui->languageComboBox->setCurrentIndex(3);
    } else if (pref.translation == "pt_PT") {
        ui->languageComboBox->setCurrentIndex(4);
    } else if (pref.translation == "fr_FR") {
        ui->languageComboBox->setCurrentIndex(5);
    } else if (pref.translation == "de_DE") {
        ui->languageComboBox->setCurrentIndex(6);
    } else if (pref.translation == "ko_KR") {
        ui->languageComboBox->setCurrentIndex(7);
    }

    // Set check updates
    if (pref.checkUpdates.has_value()) {
        ui->checkForUpdatesCheckBox->setChecked(pref.checkUpdates.value());
    }

    // Set action when done
    switch (pref.actionWhenDone) {
    case Video2XPreferences::ActionWhenDone::DoNothing:
        ui->whenDoneComboBox->setCurrentIndex(0);
        break;
    case Video2XPreferences::ActionWhenDone::Shutdown:
        ui->whenDoneComboBox->setCurrentIndex(1);
        break;
    case Video2XPreferences::ActionWhenDone::Sleep:
        ui->whenDoneComboBox->setCurrentIndex(2);
        break;
    case Video2XPreferences::ActionWhenDone::Hibernate:
        ui->whenDoneComboBox->setCurrentIndex(3);
        break;
    }

    // Set auto show stats
    ui->autoShowStatsCheckBox->setChecked(pref.autoShowStats);

    // Set remove finished tasks
    ui->removeFinishedTasksCheckBox->setChecked(pref.removeFinishedTasks);
}

void PreferencesDialog::on_openConfigurationFileDirectoryPushButton_clicked()
{
    std::optional<std::filesystem::path> configDir = getConfigDir();
    if (!configDir.has_value()) {
        return;
    }

#ifdef _WIN32
    QString configDirStr = QString::fromStdWString(configDir.value().wstring());
#else
    QString configDirStr = QString::fromStdString(configDir.value().string());
#endif

    QDesktopServices::openUrl(QUrl::fromLocalFile(configDirStr));
}

void PreferencesDialog::on_resetAllPreferencesPushButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Restore Default Settings"));
    msgBox.setText(tr("Are you sure you want to erase all custom settings?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

    if (reply == QMessageBox::Yes) {
        m_prefManager.resetPreferences();
        setPreferences(m_prefManager.getPreferences());
        QMessageBox::information(this,
                                 tr("Default Settings Restored"),
                                 tr("The default settings have been restored.\nRestart the "
                                    "application for changes to take effect."));
        reject();
    }
}
