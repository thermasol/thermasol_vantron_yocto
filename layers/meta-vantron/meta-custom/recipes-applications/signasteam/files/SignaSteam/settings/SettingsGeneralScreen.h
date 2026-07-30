#ifndef SETTINGSGENERALSCREEN_H
#define SETTINGSGENERALSCREEN_H

#include "application/AppConstants.h"

#include <QWidget>

namespace Ui {
    class SettingsGeneralScreenLandscape;
    class SettingsGeneralScreenPortrait;
}

class SettingsGeneralScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsGeneralScreen(QWidget *parent = nullptr);
    ~SettingsGeneralScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

private slots:
    void onSettingsGeneralBtnClicked(int);

private:
    QString orientationSelected;
    Ui::SettingsGeneralScreenLandscape *uiLandscape;
    Ui::SettingsGeneralScreenPortrait *uiPortrait;
};

#endif // SETTINGSGENERALSCREEN_H
