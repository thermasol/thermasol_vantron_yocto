#include "SettingsScreen.h"
#include "ui_SettingsScreen-landscape.h"
#include "ui_SettingsScreen-portrait.h"

#include "SettingsScreenModel.h"
#include "SettingsGeneralScreen.h"
#include "settings_device/SettingsOrientationScreen.h"
#include "settings_device/SettingsRebootScreen.h"
#include "settings_device/SettingsAboutScreen.h"
#include "settings_device/SettingsSetTimeScreen.h"
#include "settings_user/SettingsUsernameKeyboardScreen.h"
#include "settings_user/SettingsTempScaleScreen.h"
#include "settings_user/SettingsTimeFormatScreen.h"
#include "settings_user/SettingsUsernameScreen.h"
#include "settings_equipment/SettingsEquipmentScreen.h"
#include "settings_equipment/EquipmentCanInterface.h"
#include "settings_equipment/SettingsSteamGeneratorScreen.h"
#include "settings_equipment/SettingsDigitalShowerScreen.h"
#include "settings_equipment/SettingsSlsrScreen.h"
#include "settings_equipment/SettingsDiagnosticsScreen.h"

#include <QDebug>

SettingsScreen::SettingsScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel,
    EquipmentCanInterface *equipmentCanInterface, BluetoothCanInterface *bluetoothCanInterface) :
    QWidget(parent),
    settingsUsernameKeyboardScreen(new SettingsUsernameKeyboardScreen(this, settingsScreenModel)),
    settingsGeneralScreen(new SettingsGeneralScreen(this)),
    settingsTempScaleScreen(new SettingsTempScaleScreen(this, settingsScreenModel)),
    settingsTimeFormatScreen(new SettingsTimeFormatScreen(this, settingsScreenModel)),
    settingsSetTimeScreen(new SettingsSetTimeScreen(this, settingsScreenModel)),
    settingsUsernameScreen(new SettingsUsernameScreen(this, settingsScreenModel)),
    settingsEquipmentScreen(new SettingsEquipmentScreen(this, equipmentCanInterface)),
    settingsOrientationScreen(new SettingsOrientationScreen(this, settingsScreenModel)),
    settingsRebootScreen(new SettingsRebootScreen(this)),
    settingsAboutScreen(new SettingsAboutScreen(this)),
    settingsSteamGeneratorScreen(new SettingsSteamGeneratorScreen(this, equipmentCanInterface, settingsScreenModel)),
    settingsDigitalShowerScreen(new SettingsDigitalShowerScreen(this, equipmentCanInterface)),
    settingsSlsrScreen(new SettingsSlsrScreen(this, settingsScreenModel, bluetoothCanInterface, equipmentCanInterface)),
    settingsDiagnosticsScreen(new SettingsDiagnosticsScreen(this, equipmentCanInterface))
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsScreenPortrait;
        uiPortrait->setupUi(this);
    }

    settingsScreenContainer = findChild<QStackedWidget*>("settingsScreenContainer");
    settingsScreenContainer->addWidget(settingsGeneralScreen);
    settingsScreenContainer->addWidget(settingsTempScaleScreen);
    settingsScreenContainer->addWidget(settingsTimeFormatScreen);
    settingsScreenContainer->addWidget(settingsSetTimeScreen);
    settingsScreenContainer->addWidget(settingsUsernameScreen);
    settingsScreenContainer->addWidget(settingsEquipmentScreen);
    settingsScreenContainer->addWidget(settingsOrientationScreen);
    settingsScreenContainer->addWidget(settingsRebootScreen);
    settingsScreenContainer->addWidget(settingsAboutScreen);
    settingsScreenContainer->addWidget(settingsSteamGeneratorScreen);
    settingsScreenContainer->addWidget(settingsDigitalShowerScreen);
    settingsScreenContainer->addWidget(settingsSlsrScreen);
    settingsScreenContainer->addWidget(settingsDiagnosticsScreen);
    settingsScreenContainer->addWidget(settingsUsernameKeyboardScreen);



    connect(settingsGeneralScreen, &SettingsGeneralScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsTempScaleScreen, &SettingsTempScaleScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsTimeFormatScreen, &SettingsTimeFormatScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsSetTimeScreen, &SettingsSetTimeScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsUsernameScreen, &SettingsUsernameScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsEquipmentScreen, &SettingsEquipmentScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsOrientationScreen, &SettingsOrientationScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsRebootScreen, &SettingsRebootScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsAboutScreen, &SettingsAboutScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsSteamGeneratorScreen, &SettingsSteamGeneratorScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsDigitalShowerScreen, &SettingsDigitalShowerScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsSlsrScreen, &SettingsSlsrScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsDiagnosticsScreen, &SettingsDiagnosticsScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);
    connect(settingsUsernameKeyboardScreen, &SettingsUsernameKeyboardScreen::settingsScreenChanged, this, &SettingsScreen::onSettingsScreenChanged);

    connect(settingsAboutScreen, &SettingsAboutScreen::showModeBtnClicked, equipmentCanInterface, &EquipmentCanInterface::onShowModeBtnClicked);

    connect(this, &SettingsScreen::userLoggedOut, settingsSteamGeneratorScreen, &SettingsSteamGeneratorScreen::onUserLoggedOut);
    connect(settingsEquipmentScreen, &SettingsEquipmentScreen::equipmentDisconnected, settingsSteamGeneratorScreen, &SettingsSteamGeneratorScreen::onEquipmentDisconnected);
    connect(equipmentCanInterface, &EquipmentCanInterface::tempSensorRead, settingsDiagnosticsScreen, &SettingsDiagnosticsScreen::onTempSensorRead);

    connect(settingsUsernameKeyboardScreen, &SettingsUsernameKeyboardScreen::usernameEntered, settingsUsernameScreen, &SettingsUsernameScreen::onUsernameChanged);
    connect(settingsUsernameKeyboardScreen, &SettingsUsernameKeyboardScreen::usernameEntered, settingsScreenModel, &SettingsScreenModel::onUsernameEntered);

    settingsSteamGeneratorScreen->onFastStartUiChanged(FastStart::GREEN); //TODO Find out a way to where app persistence is used to set default
}

SettingsScreen::~SettingsScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsScreen::onSettingsScreenChanged(SettingsScreenAll screenClickedId)
{
    switch (screenClickedId) {
        case SettingsScreenAll::GENERAL:
            settingsScreenContainer->setCurrentWidget(settingsGeneralScreen);
            break;
        case SettingsScreenAll::TEMP_SCALE:
            settingsScreenContainer->setCurrentWidget(settingsTempScaleScreen);
            break;
        case SettingsScreenAll::TIME_FORMAT:
            settingsScreenContainer->setCurrentWidget(settingsTimeFormatScreen);
            break;
        case SettingsScreenAll::SET_TIME:
            settingsScreenContainer->setCurrentWidget(settingsSetTimeScreen);
            break;
        case SettingsScreenAll::USERNAMES:
            settingsScreenContainer->setCurrentWidget(settingsUsernameScreen);
            break;
        case SettingsScreenAll::EQUIPMENT:
            settingsScreenContainer->setCurrentWidget(settingsEquipmentScreen);
            break;
        case SettingsScreenAll::ORIENTATION:
            settingsScreenContainer->setCurrentWidget(settingsOrientationScreen);
            break;
        case SettingsScreenAll::REBOOT:
            settingsScreenContainer->setCurrentWidget(settingsRebootScreen);
            break;
        case SettingsScreenAll::ABOUT:
            settingsScreenContainer->setCurrentWidget(settingsAboutScreen);
            break;
        case SettingsScreenAll::STEAM_GENERATOR:
            settingsScreenContainer->setCurrentWidget(settingsSteamGeneratorScreen);
            break;
        case SettingsScreenAll::DIGITAL_SHOWER:
            settingsScreenContainer->setCurrentWidget(settingsDigitalShowerScreen);
            break;
        case SettingsScreenAll::SLSR:
            settingsScreenContainer->setCurrentWidget(settingsSlsrScreen);
            break;
        case SettingsScreenAll::DIAGNOSTICS:
            settingsScreenContainer->setCurrentWidget(settingsDiagnosticsScreen);
            break;
        case SettingsScreenAll::KEYBOARD:
            settingsScreenContainer->setCurrentWidget(settingsUsernameKeyboardScreen);
            break;
    }
}

void SettingsScreen::onPfBtnClicked()
{
    onSettingsScreenChanged(SettingsScreenAll::STEAM_GENERATOR);
}
