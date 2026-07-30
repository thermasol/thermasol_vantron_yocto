#include "SettingsRebootScreen.h"
#include "ui_SettingsRebootScreen-landscape.h"
#include "ui_SettingsRebootScreen-portrait.h"

#include "testing/TestBuild.h"

#include <QSettings>
#include <QProcess>

#include <QDebug>

SettingsRebootScreen::SettingsRebootScreen(QWidget *parent) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsRebootScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsRebootScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsRebootScreen::onBackButtonClicked);

    QToolButton* rebootDeviceBtnClickArea = findChild<QToolButton*>("rebootDeviceBtnClickArea");
    connect(rebootDeviceBtnClickArea, &QToolButton::clicked, this, &SettingsRebootScreen::onRebootBtnClicked);
}

SettingsRebootScreen::~SettingsRebootScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsRebootScreen::onRebootBtnClicked()
{
    if(!testBuild::isTestBuild) {
        QProcess::execute(REBOOT_PACKAGE);
    }
}

void SettingsRebootScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}
