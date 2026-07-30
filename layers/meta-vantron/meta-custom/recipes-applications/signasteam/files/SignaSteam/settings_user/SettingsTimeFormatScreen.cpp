#include "SettingsTimeFormatScreen.h"
#include "ui_SettingsTimeFormatScreen-landscape.h"
#include "ui_SettingsTimeFormatScreen-portrait.h"

#include "settings/SettingsScreenModel.h"

#include <QDebug>

SettingsTimeFormatScreen::SettingsTimeFormatScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsTimeFormatScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsTimeFormatScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsTimeFormatScreen::onBackButtonClicked);

    QButtonGroup* timeFormatButtonGroup = findChild<QButtonGroup*>("timeFormatButtonGroup");
    twelveHourFormatBtn = findChild<QToolButton*>("twelveHourFormatBtn");
    timeFormatButtonGroup->setId(twelveHourFormatBtn, static_cast<int>(TimeFormat::TWELVE_HOUR));

    twentyfourHourFormatBtn = findChild<QToolButton*>("twentyfourHourFormatBtn");
    timeFormatButtonGroup->setId(twentyfourHourFormatBtn, static_cast<int>(TimeFormat::TWENTY_FOUR_HOUR));

    connect(timeFormatButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), settingsScreenModel, &SettingsScreenModel::onTimeFormatBtnClicked);
    connect(settingsScreenModel, &SettingsScreenModel::timeFormatChanged, this, &SettingsTimeFormatScreen::onTimeFormatChanged);
}

SettingsTimeFormatScreen::~SettingsTimeFormatScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsTimeFormatScreen::onTimeFormatChanged(QString timeFormat)
{
    if(timeFormat == appPersistence::TIME_FORMAT_TWELVE_HOUR_VALUE) {
        twelveHourFormatBtn->setChecked(true);
    } else if(timeFormat == appPersistence::TIME_FORMAT_TWENTYFOUR_HOUR_VALUE) {
        twentyfourHourFormatBtn->setChecked(true);
    }
}

void SettingsTimeFormatScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}
