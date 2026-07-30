#include "SettingsGeneralScreen.h"
#include "ui_SettingsGeneralScreen-landscape.h"
#include "ui_SettingsGeneralScreen-portrait.h"

#include <QSettings>

#include <QDebug>

SettingsGeneralScreen::SettingsGeneralScreen(QWidget *parent) : QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsGeneralScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsGeneralScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QButtonGroup* settingsGeneralGroup = findChild<QButtonGroup*>("settingsGeneralGroup");

    QToolButton* tempScaleBtn = findChild<QToolButton*>("tempScaleBtnClickArea");
    settingsGeneralGroup->setId(tempScaleBtn, static_cast<int>(SettingsScreenAll::TEMP_SCALE));

    QToolButton* timeFormatBtn = findChild<QToolButton*>("timeFormatBtnClickArea");
    settingsGeneralGroup->setId(timeFormatBtn, static_cast<int>(SettingsScreenAll::TIME_FORMAT));

    QToolButton* setTimeBtn = findChild<QToolButton*>("setTimeBtnClickArea");
    settingsGeneralGroup->setId(setTimeBtn, static_cast<int>(SettingsScreenAll::SET_TIME));

    QToolButton* userNamesBtn = findChild<QToolButton*>("userNamesBtnClickArea");
    settingsGeneralGroup->setId(userNamesBtn, static_cast<int>(SettingsScreenAll::USERNAMES));

    QToolButton* equipmentBtn = findChild<QToolButton*>("equipmentBtnClickArea");
    settingsGeneralGroup->setId(equipmentBtn, static_cast<int>(SettingsScreenAll::EQUIPMENT));

    QToolButton* orientationBtn = findChild<QToolButton*>("orientationBtnClickArea");
    settingsGeneralGroup->setId(orientationBtn, static_cast<int>(SettingsScreenAll::ORIENTATION));

    QToolButton* rebootBtn = findChild<QToolButton*>("rebootBtnClickArea");
    settingsGeneralGroup->setId(rebootBtn, static_cast<int>(SettingsScreenAll::REBOOT));

    QToolButton* aboutBtn = findChild<QToolButton*>("aboutBtnClickArea");
    settingsGeneralGroup->setId(aboutBtn, static_cast<int>(SettingsScreenAll::ABOUT));

    connect(settingsGeneralGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &SettingsGeneralScreen::onSettingsGeneralBtnClicked);
}

SettingsGeneralScreen::~SettingsGeneralScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsGeneralScreen::onSettingsGeneralBtnClicked(int buttonClickedAsInt)
{
    SettingsScreenAll buttonClickedId = static_cast<SettingsScreenAll>(buttonClickedAsInt);
    emit settingsScreenChanged(buttonClickedId);
}
