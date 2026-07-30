#ifndef SETTINGSORIENTATIONSCREEN_H
#define SETTINGSORIENTATIONSCREEN_H

#include "application/AppConstants.h"

#include <QToolButton>

namespace Ui {
    class SettingsOrientationScreenLandscape;
    class SettingsOrientationScreenPortrait;
}

class SettingsScreenModel;

class SettingsOrientationScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsOrientationScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr);
    ~SettingsOrientationScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);
    void orientationChanged(Orientation);

private slots:
    void onBackButtonClicked();
    void onOrientationBtnClicked(int);

private:
    Ui::SettingsOrientationScreenLandscape *uiLandscape;
    Ui::SettingsOrientationScreenPortrait *uiPortrait;
    QString orientationSelected;
    QToolButton *portraitBtnLabel;
    QToolButton *landscapeBtnLabel;
};

#endif // SETTINGSORIENTATIONSCREEN_H
