#include "SettingsEquipmentScreen.h"
#include "ui_SettingsEquipmentScreen-landscape.h"
#include "ui_SettingsEquipmentScreen-portrait.h"

#include "EquipmentCanInterface.h"
#include "testing/TestBuild.h"

#include <QSettings>

#include <QDebug>

SettingsEquipmentScreen::SettingsEquipmentScreen(QWidget *parent, EquipmentCanInterface *equipmentCanInterface) : QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsEquipmentScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsEquipmentScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsEquipmentScreen::onBackButtonClicked);

    QButtonGroup* settingsEquipmentGroup = findChild<QButtonGroup*>("settingsEquipmentGroup");

    digitalShowerBtnClickArea = findChild<QToolButton*>("digitalShowerBtnClickArea");
    digitalShowerBtnImg = findChild<QToolButton*>("digitalShowerBtnImg");
    digitalShowerBtnLabel = findChild<QToolButton*>("digitalShowerBtnLabel");
    settingsEquipmentGroup->setId(digitalShowerBtnClickArea, static_cast<int>(SettingsScreenAll::DIGITAL_SHOWER));

    steamGeneratorBtnClickArea = findChild<QToolButton*>("steamGeneratorBtnClickArea");
    steamGeneratorBtnImg = findChild<QToolButton*>("steamGeneratorBtnImg");
    steamGeneratorBtnLabel = findChild<QToolButton*>("steamGeneratorBtnLabel");
    settingsEquipmentGroup->setId(steamGeneratorBtnClickArea, static_cast<int>(SettingsScreenAll::STEAM_GENERATOR));

    slsrBtnClickArea = findChild<QToolButton*>("slsrBtnClickArea");
    slsrBtnImg = findChild<QToolButton*>("slsrBtnImg");
    slsrBtnLabel = findChild<QToolButton*>("slsrBtnLabel");
    hvslrBtnLabel = findChild<QToolButton*>("hvslrBtnLabel");
    settingsEquipmentGroup->setId(slsrBtnClickArea, static_cast<int>(SettingsScreenAll::SLSR));

    QToolButton* diagnosticsBtnClickArea = findChild<QToolButton*>("diagnosticsBtnClickArea");
    settingsEquipmentGroup->setId(diagnosticsBtnClickArea, static_cast<int>(SettingsScreenAll::DIAGNOSTICS));

    QToolButton* diagnosticsBtnImg = findChild<QToolButton*>("diagnosticsBtnImg");
    QLabel* diagnosticsBtnLabel = findChild<QLabel*>("diagnosticsBtnLabel");

    if(!testBuild::isTestBuild) {
        diagnosticsBtnClickArea->hide();
        diagnosticsBtnImg->hide();
        diagnosticsBtnLabel->hide();
    }

    connect(settingsEquipmentGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &SettingsEquipmentScreen::onSettingsEquipmentBtnClicked);
    connect(equipmentCanInterface, &EquipmentCanInterface::canDevicesStateChanged, this, &SettingsEquipmentScreen::onCanDevicesStateChanged);
}

SettingsEquipmentScreen::~SettingsEquipmentScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsEquipmentScreen::onCanDevicesStateChanged(QMap<CanDevice, CanDeviceState> canDevices)
{
    QMap<CanDevice, CanDeviceState>::iterator iterator;
    for (iterator = canDevices.begin(); iterator != canDevices.end(); ++iterator) {
        switch(iterator.key()) {
            case CanDevice::DIGITAL_SHOWER:
                if(iterator.value() == CanDeviceState::PRESENT) {
                    digitalShowerBtnClickArea->setEnabled(true);
                    digitalShowerBtnImg->setEnabled(true);
                    digitalShowerBtnLabel->setEnabled(true);
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                    digitalShowerBtnClickArea->setEnabled(false);
                    digitalShowerBtnImg->setEnabled(false);
                    digitalShowerBtnLabel->setEnabled(false);
                }
                break;
            case CanDevice::STEAM_GENERATOR:
                if(iterator.value() == CanDeviceState::PRESENT) {
                    steamGeneratorBtnClickArea->setEnabled(true);
                    steamGeneratorBtnImg->setEnabled(true);
                    steamGeneratorBtnLabel->setEnabled(true);
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                    steamGeneratorBtnClickArea->setEnabled(false);
                    steamGeneratorBtnImg->setEnabled(false);
                    steamGeneratorBtnLabel->setEnabled(false);
                    emit equipmentDisconnected();
                }
                break;
            case CanDevice::HVSLSR:
                slsrBtnLabel->hide();
                hvslrBtnLabel->show();
                slsrBtnClickArea->setEnabled(false);  // currently no settings for HVSLR
                if(iterator.value() == CanDeviceState::PRESENT) {
                    slsrBtnImg->setEnabled(true);
                    hvslrBtnLabel->setEnabled(true);
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                    slsrBtnImg->setEnabled(false);
                    hvslrBtnLabel->setEnabled(false);
                }


            break;
            case CanDevice::SLSR:
                hvslrBtnLabel->hide();
                slsrBtnLabel->show();
                if(iterator.value() == CanDeviceState::PRESENT) {
                    slsrBtnClickArea->setEnabled(true);
                    slsrBtnImg->setEnabled(true);
                    slsrBtnLabel->setEnabled(true);
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                    slsrBtnClickArea->setEnabled(false);
                    slsrBtnImg->setEnabled(false);
                    slsrBtnLabel->setEnabled(false);
                }
                break;
        }
    }
}

void SettingsEquipmentScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}

void SettingsEquipmentScreen::onSettingsEquipmentBtnClicked(int buttonClickedAsInt)
{
    SettingsScreenAll buttonClickedId = static_cast<SettingsScreenAll>(buttonClickedAsInt);
    emit settingsScreenChanged(buttonClickedId);
}

