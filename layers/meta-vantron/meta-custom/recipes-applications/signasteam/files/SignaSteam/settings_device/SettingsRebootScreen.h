#ifndef SETTINGSREBOOTSCREEN_H
#define SETTINGSREBOOTSCREEN_H

#include "application/AppConstants.h"

#include <QWidget>

namespace Ui {
    class SettingsRebootScreenLandscape;
    class SettingsRebootScreenPortrait;
}

class SettingsRebootScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsRebootScreen(QWidget *parent = nullptr);
    ~SettingsRebootScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

private slots:
    void onRebootBtnClicked();
    void onBackButtonClicked();

private:
    const QString REBOOT_PACKAGE = "/sbin/reboot";
    Ui::SettingsRebootScreenLandscape *uiLandscape;
    Ui::SettingsRebootScreenPortrait *uiPortrait;
    QString orientationSelected;
};

#endif // SETTINGSREBOOTSCREEN_H
