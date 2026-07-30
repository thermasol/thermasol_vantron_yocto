#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include "application/AppConstants.h"

#include <QStackedWidget>

namespace Ui {
    class SettingsScreenLandscape;
    class SettingsScreenPortrait;
}

class SettingsUsernameKeyboardScreen;
class EquipmentCanInterface;
class BluetoothCanInterface;
class SettingsGeneralScreen;
class SettingsTempScaleScreen;
class SettingsTimeFormatScreen;
class SettingsSetTimeScreen;
class SettingsEquipmentScreen;
class SettingsUsernameScreen;
class SettingsOrientationScreen;
class SettingsRebootScreen;
class SettingsAboutScreen;
class SettingsSteamGeneratorScreen;
class SettingsDigitalShowerScreen;
class SettingsSlsrScreen;
class SettingsDiagnosticsScreen;
class SettingsScreenModel;

class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr,
                            EquipmentCanInterface *equipmentCanInterface = nullptr, BluetoothCanInterface *bluetoothCanInterface = nullptr);
    ~SettingsScreen();

signals:
    void userLoggedOut();
    void tempSensorRead(double);

public slots:
    void onSettingsScreenChanged(SettingsScreenAll);
    void onPfBtnClicked();

private:
    Ui::SettingsScreenLandscape *uiLandscape;
    Ui::SettingsScreenPortrait *uiPortrait;
    SettingsUsernameKeyboardScreen *settingsUsernameKeyboardScreen;
    SettingsScreenModel *settingsScreenModel;
    SettingsGeneralScreen *settingsGeneralScreen;
    SettingsTempScaleScreen *settingsTempScaleScreen;
    SettingsTimeFormatScreen *settingsTimeFormatScreen;
    SettingsSetTimeScreen *settingsSetTimeScreen;
    SettingsUsernameScreen *settingsUsernameScreen;
    SettingsEquipmentScreen *settingsEquipmentScreen;
    SettingsOrientationScreen *settingsOrientationScreen;
    SettingsRebootScreen *settingsRebootScreen;
    SettingsAboutScreen *settingsAboutScreen;
    SettingsSteamGeneratorScreen *settingsSteamGeneratorScreen;
    SettingsDigitalShowerScreen *settingsDigitalShowerScreen;
    SettingsSlsrScreen *settingsSlsrScreen;
    SettingsDiagnosticsScreen *settingsDiagnosticsScreen;
    QString orientationSelected;
    QStackedWidget* settingsScreenContainer;
};

#endif // SETTINGSSCREEN_H
