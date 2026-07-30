#include "SettingsDigitalShowerScreen.h"
#include "ui_SettingsDigitalShowerScreen-landscape.h"
#include "ui_SettingsDigitalShowerScreen-portrait.h"

#include "EquipmentCanInterface.h"

#include <QSettings>

#include <QDebug>

SettingsDigitalShowerScreen::SettingsDigitalShowerScreen(QWidget *parent, EquipmentCanInterface *equipmentCanInterface) :
    QWidget(parent)
{
    this->equipmentCanInterface = equipmentCanInterface;

    int fontSize = 0;
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsDigitalShowerScreenLandscape;
        uiLandscape->setupUi(this);
        fontSize = 30;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsDigitalShowerScreenPortrait;
        uiPortrait->setupUi(this);
        fontSize = 20;
    }

    QLabel *dongleFirmwareVersionTitle = findChild<QLabel*>("dongleFirmwareVersionTitle");
    dongleFirmwareVersionTitle->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));

    QLabel *valveConnectionStatusTitle = findChild<QLabel*>("valveConnectionStatusTitle");
    valveConnectionStatusTitle->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));

    QLabel *valvePowerStatusTitle = findChild<QLabel*>("valvePowerStatusTitle");
    valvePowerStatusTitle->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));

    QLabel *totalFlowTitle = findChild<QLabel*>("totalFlowTitle");
    totalFlowTitle->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));

    QLabel *valveMeasuredTempTitle = findChild<QLabel*>("valveMeasuredTempTitle");
    valveMeasuredTempTitle->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsDigitalShowerScreen::onBackButtonClicked);

    dongleFirmwareVersion = findChild<QLabel*>("dongleFirmwareVersion");
    dongleFirmwareVersion->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    connect(equipmentCanInterface, &EquipmentCanInterface::dongleFirmwareVersion, this, &SettingsDigitalShowerScreen::updateDongleFirmwareVersion);

    valveConnectionStatus = findChild<QLabel*>("valveConnectionStatus");
    valveConnectionStatus->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    connect(equipmentCanInterface, &EquipmentCanInterface::valveConnectionStatus, this, &SettingsDigitalShowerScreen::updateValveConnectionStatus);

    valvePowerStatus = findChild<QLabel*>("valvePowerStatus");
    valvePowerStatus->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    connect(equipmentCanInterface, &EquipmentCanInterface::currentPowerStatus, this, &SettingsDigitalShowerScreen::updateValvePowerStatus);

    totalFlow = findChild<QLabel*>("totalFlow");
    totalFlow->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    connect(equipmentCanInterface, &EquipmentCanInterface::waterFlowCount, this, &SettingsDigitalShowerScreen::updateTotalFlow);

    valveMeasuredTemp = findChild<QLabel*>("valveMeasuredTemp");
    valveMeasuredTemp->setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    connect(equipmentCanInterface, &EquipmentCanInterface::valveMeasuredTemp, this, &SettingsDigitalShowerScreen::updateValveMeasuredTemp);
}

SettingsDigitalShowerScreen::~SettingsDigitalShowerScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsDigitalShowerScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::EQUIPMENT);
}

void SettingsDigitalShowerScreen::updateDongleFirmwareVersion(int firmwareVersion)
{
    if(firmwareVersion == DEFAULT_CAN_FIRMWARE_VERSION) {
        dongleFirmwareVersion->setText(NOT_AVAILABLE);
    } else {
        dongleFirmwareVersion->setText(QString::number(firmwareVersion));
    }
}

void SettingsDigitalShowerScreen::updateValvePowerStatus(ShowerValvePowerStatus powerStatus)
{
    switch (powerStatus) {
        case ShowerValvePowerStatus::ON_LINE_POWER:
            valvePowerStatus->setText(POWER_STATUS_LINE);
            break;
        case ShowerValvePowerStatus::ON_BATTERY_POWER:
            valvePowerStatus->setText(POWER_STATUS_BATTERY);
            break;
        case ShowerValvePowerStatus::BATTERY_POWER_LOW:
            valvePowerStatus->setText(POWER_STATUS_BATTERY_LOW);
            break;
        case ShowerValvePowerStatus::NO_POWER_INFO_FROM_VALVE:
            valvePowerStatus->setText(NOT_AVAILABLE);
            break;
    }
}

void SettingsDigitalShowerScreen::updateValveConnectionStatus(bool isConnected)
{
    if(isConnected) {
        valveConnectionStatus->setText(CONNECTED_CONNECTION_STATUS);
    } else {
        valveConnectionStatus->setText(DISCONNECTED_CONNECTION_STATUS);
    }
}

void SettingsDigitalShowerScreen::updateTotalFlow(int totalFlowValue)
{
    if(totalFlowValue == DEFAULT_CAN_FLOW_VALUE) {
        totalFlow->setText(NOT_AVAILABLE);
    } else {
        totalFlow->setText(QString::number(totalFlowValue));
    }
}

void SettingsDigitalShowerScreen::updateValveMeasuredTemp(int waterTemp)
{
    if(waterTemp == DEFAULT_CAN_TEMP_VALUE) {
        valveMeasuredTemp->setText(NOT_AVAILABLE);
    } else {
        valveMeasuredTemp->setText(QString::number(waterTemp));
    }
}
