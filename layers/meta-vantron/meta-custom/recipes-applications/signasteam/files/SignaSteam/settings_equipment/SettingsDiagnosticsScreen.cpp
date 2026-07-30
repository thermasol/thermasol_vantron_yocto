#include "SettingsDiagnosticsScreen.h"
#include "ui_SettingsDiagnosticsScreen-landscape.h"
#include "ui_SettingsDiagnosticsScreen-portrait.h"

#include "EquipmentCanInterface.h"

#include <QSettings>

#include <QDebug>

SettingsDiagnosticsScreen::SettingsDiagnosticsScreen(QWidget *parent, EquipmentCanInterface *equipmentCanInterface) :
    QWidget(parent)
{
    this->equipmentCanInterface = equipmentCanInterface;

    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsDiagnosticsScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsDiagnosticsScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsDiagnosticsScreen::onBackButtonClicked);

    tempSensorReading = findChild<QLabel*>("tempSensorReading");

    QToolButton* helloMidButton = findChild<QToolButton*>("helloMidButton");
    connect(helloMidButton, &QToolButton::clicked, this, &SettingsDiagnosticsScreen::onToolButtonClicked);
}

SettingsDiagnosticsScreen::~SettingsDiagnosticsScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsDiagnosticsScreen::onTempSensorRead(double tempReading)
{
    tempSensorReading->setText(QString::number(tempReading));
}

void SettingsDiagnosticsScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::EQUIPMENT);
}

void SettingsDiagnosticsScreen::onToolButtonClicked()
{
    equipmentCanInterface->diagnosticsSendHello();
}
