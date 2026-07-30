#ifndef SETTINGSDIGITALSHOWERSCREEN_H
#define SETTINGSDIGITALSHOWERSCREEN_H

#include "application/AppConstants.h"

#include <QLabel>
#include <QWidget>

namespace Ui {
    class SettingsDigitalShowerScreenLandscape;
    class SettingsDigitalShowerScreenPortrait;
}

class EquipmentCanInterface;

class SettingsDigitalShowerScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsDigitalShowerScreen(QWidget *parent = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    ~SettingsDigitalShowerScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

public slots:
    void updateDongleFirmwareVersion(int);
    void updateValvePowerStatus(ShowerValvePowerStatus);
    void updateValveConnectionStatus(bool);
    void updateTotalFlow(int);
    void updateValveMeasuredTemp(int);

private slots:
    void onBackButtonClicked();

private:
    const int DEFAULT_CAN_FIRMWARE_VERSION = 255;
    const int DEFAULT_CAN_FLOW_VALUE = -1;
    const int DEFAULT_CAN_TEMP_VALUE = 0;
    const QString POWER_STATUS_LINE = "Line";
    const QString POWER_STATUS_BATTERY = "Battery";
    const QString POWER_STATUS_BATTERY_LOW = "Battery Low";
    const QString CONNECTED_CONNECTION_STATUS = "Connected";
    const QString DISCONNECTED_CONNECTION_STATUS = "Disconnected";
    const QString NOT_AVAILABLE = "N/A";
    Ui::SettingsDigitalShowerScreenLandscape *uiLandscape;
    Ui::SettingsDigitalShowerScreenPortrait *uiPortrait;
    EquipmentCanInterface *equipmentCanInterface;
    QLabel *dongleFirmwareVersion;
    QLabel *valvePowerStatus;
    QLabel *valveConnectionStatus;
    QLabel *totalFlow;
    QLabel *valveMeasuredTemp;
    QString orientationSelected;
};

#endif // SETTINGSDIGITALSHOWERSCREEN_H
