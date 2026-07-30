#ifndef SETTINGSDIAGNOSTICSSCREEN_H
#define SETTINGSDIAGNOSTICSSCREEN_H

#include "application/AppConstants.h"

#include <QLabel>

namespace Ui {
    class SettingsDiagnosticsScreenLandscape;
    class SettingsDiagnosticsScreenPortrait;
}

class EquipmentCanInterface;

class SettingsDiagnosticsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsDiagnosticsScreen(QWidget *parent = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    ~SettingsDiagnosticsScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

public slots:
    void onTempSensorRead(double);

private slots:
    void onBackButtonClicked();
    void onToolButtonClicked();

private:
    Ui::SettingsDiagnosticsScreenLandscape *uiLandscape;
    Ui::SettingsDiagnosticsScreenPortrait *uiPortrait;
    EquipmentCanInterface *equipmentCanInterface;
    QLabel *tempSensorReading;
    QString orientationSelected;
};

#endif // SETTINGSDIAGNOSTICSSCREEN_H
