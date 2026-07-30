#include "SettingsTempScaleScreen.h"
#include "ui_SettingsTempScaleScreen-landscape.h"
#include "ui_SettingsTempScaleScreen-portrait.h"

#include "settings/SettingsScreenModel.h"


SettingsTempScaleScreen::SettingsTempScaleScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsTempScaleScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsTempScaleScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsTempScaleScreen::onBackButtonClicked);

    QButtonGroup* tempScaleButtonGroup = findChild<QButtonGroup*>("tempScaleButtonGroup");
    fahrenheitScaleBtn = findChild<QToolButton*>("fahrenheitScaleBtn");
    tempScaleButtonGroup->setId(fahrenheitScaleBtn, static_cast<int>(TempScale::FAHRENHEIT));

    celciusScaleBtn = findChild<QToolButton*>("celciusScaleBtn");
    tempScaleButtonGroup->setId(celciusScaleBtn, static_cast<int>(TempScale::CELCIUS));

    connect(tempScaleButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), settingsScreenModel, &SettingsScreenModel::onTempScaleBtnClicked);
    connect(settingsScreenModel, &SettingsScreenModel::tempScaleChanged, this, &SettingsTempScaleScreen::onTempScaleChanged);
}

SettingsTempScaleScreen::~SettingsTempScaleScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsTempScaleScreen::onTempScaleChanged(QString tempScale)
{
    if(tempScale == appPersistence::TEMP_SCALE_C_VALUE) {
        celciusScaleBtn->setChecked(true);
    } else if(tempScale == appPersistence::TEMP_SCALE_F_VALUE) {
        fahrenheitScaleBtn->setChecked(true);
    }
}

void SettingsTempScaleScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}
