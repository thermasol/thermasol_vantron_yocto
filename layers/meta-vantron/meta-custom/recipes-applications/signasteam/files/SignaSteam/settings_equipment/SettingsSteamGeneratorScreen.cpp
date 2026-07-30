#include "SettingsSteamGeneratorScreen.h"
#include "ui_SettingsSteamGeneratorScreen-landscape.h"
#include "ui_SettingsSteamGeneratorScreen-portrait.h"

#include "EquipmentCanInterface.h"
#include "settings/SettingsScreenModel.h"
#include "steam_control/SteamCanInterface.h"

#include <QSettings>

#include <QDebug>

SettingsSteamGeneratorScreen::SettingsSteamGeneratorScreen(QWidget *parent, EquipmentCanInterface *equipmentCanInterface, SettingsScreenModel *settingsScreenModel, SteamCanInterface *steamCanInterface) :
    QWidget(parent)
{
    this->equipmentCanInterface = equipmentCanInterface;
    this->settingsScreenModel = settingsScreenModel;
    this->steamCanInterface = steamCanInterface;

    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsSteamGeneratorScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsSteamGeneratorScreenPortrait;
        uiPortrait->setupUi(this);
    }

    backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsSteamGeneratorScreen::onBackButtonClicked);

    fastStartBtnGroup = findChild<QButtonGroup*>("fastStartBtnGroup");
    connect(fastStartBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), settingsScreenModel, &SettingsScreenModel::onFastStartChanged);
    connect(settingsScreenModel, &SettingsScreenModel::fastStartChanged, this, &SettingsSteamGeneratorScreen::onFastStartUiChanged);

    fastStartOffBtn = findChild<QToolButton*>("fastStartOffBtn");
    fastStartBtnGroup->setId(fastStartOffBtn, static_cast<int>(FastStart::OFF));

    fastStartOnBtn = findChild<QToolButton*>("fastStartOnBtn");
    fastStartBtnGroup->setId(fastStartOnBtn, static_cast<int>(FastStart::ON));

    fastStartGreenBtn = findChild<QToolButton*>("fastStartGreenBtn");
    fastStartBtnGroup->setId(fastStartGreenBtn, static_cast<int>(FastStart::GREEN));

    startStopPowerFlushBtn = findChild<QToolButton*>("startStopPowerFlushBtn");
    connect(startStopPowerFlushBtn, &QToolButton::toggled, this, &SettingsSteamGeneratorScreen::onPowerFlushBtnToggled);

    powerFlushMinsLeft = findChild<QLabel*>("powerFlushMinsLeft");
    powerFlushMinsLeft->setFont(fontStyle::getQuicksandBoldWithSize(FONT_SIZE));

    powerFlushSecsLeft = findChild<QLabel*>("powerFlushSecsLeft");
    powerFlushSecsLeft->setFont(fontStyle::getQuicksandBoldWithSize(FONT_SIZE));
    connect(equipmentCanInterface, &EquipmentCanInterface::pfTimerChanged, this, &SettingsSteamGeneratorScreen::onPfTimerChanged);
    connect(equipmentCanInterface, &EquipmentCanInterface::psbStateChanged, this, &SettingsSteamGeneratorScreen::onPfFinished);
}

SettingsSteamGeneratorScreen::~SettingsSteamGeneratorScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsSteamGeneratorScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::EQUIPMENT);
}

void SettingsSteamGeneratorScreen::onFastStartUiChanged(FastStart buttonClickedAsInt)
{
    switch (buttonClickedAsInt) {
        case FastStart::OFF:
            fastStartOffBtn->setChecked(true);
            break;
        case FastStart::ON:
            fastStartOnBtn->setChecked(true);
            break;
        case FastStart::GREEN:
            fastStartGreenBtn->setChecked(true);
            break;
    }
}



void SettingsSteamGeneratorScreen::onPowerFlushBtnToggled(bool isChecked)
{
    if(isChecked) {
        fastStartGreenBtn->hide();
        fastStartOffBtn->hide();
        fastStartOnBtn->hide();
        backButton->hide();
        equipmentCanInterface->startPowerFlush();
    } else {
        fastStartGreenBtn->show();
        fastStartOffBtn->show();
        fastStartOnBtn->show();
        backButton->show();
        equipmentCanInterface->stopPowerFlush();
        steamCanInterface->stopSteamSession();
        onPfTimerChanged(0);
    }
}

void SettingsSteamGeneratorScreen::onPfTimerChanged(int timeLeft)
{
    QString minutes = QString("%1").arg(timeLeft / 60, 2, 10, QChar('0'));
    QString seconds = QString("%1").arg(timeLeft % 60, 2, 10, QChar('0'));

    powerFlushMinsLeft->setText(minutes);
    powerFlushSecsLeft->setText(seconds);
}

void SettingsSteamGeneratorScreen::onPfFinished(QPair<PsbState, PsbErrorState> psbState)
{
    if(psbState.first == PsbState::PF_FINISHED) {
        startStopPowerFlushBtn->setChecked(false);
        onPfTimerChanged(0);
    }
}

void SettingsSteamGeneratorScreen::onUserLoggedOut()
{
    if(startStopPowerFlushBtn->isChecked()) {
        startStopPowerFlushBtn->toggle();
    }
}

void SettingsSteamGeneratorScreen::onEquipmentDisconnected()
{
    onUserLoggedOut();
}

