#include "SettingsSetTimeScreen.h"
#include "ui_SettingsSetTimeScreen-landscape.h"
#include "ui_SettingsSetTimeScreen-portrait.h"

#include "settings/SettingsScreenModel.h"

#include <QDebug>

SettingsSetTimeScreen::SettingsSetTimeScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel) :
    QWidget(parent)
{
    this->settingsScreenModel = settingsScreenModel;

    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsSetTimeScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsSetTimeScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsSetTimeScreen::onBackButtonClicked);

    timeEdit = findChild<QTimeEdit*>("timeEdit");
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        timeEdit->setFont(fontStyle::getQuicksandBoldWithSize(TIME_FONT_SIZE_LAND));
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        timeEdit->setFont(fontStyle::getQuicksandBoldWithSize(TIME_FONT_SIZE_PORT));
    }
    connect(timeEdit, &QTimeEdit::timeChanged, settingsScreenModel, &SettingsScreenModel::onTimeEditChanged);
    connect(this->settingsScreenModel, &SettingsScreenModel::timeFormatChanged, this, &SettingsSetTimeScreen::onTimeFormatChanged);
    connect(this->settingsScreenModel, &SettingsScreenModel::timeEditReset, this, &SettingsSetTimeScreen::onTimeEditReset);

    QToolButton* setTimeBtn = findChild<QToolButton*>("setTimeBtn");
    connect(setTimeBtn, &QToolButton::clicked, this, &SettingsSetTimeScreen::onSetTimeBtnClicked);
}

SettingsSetTimeScreen::~SettingsSetTimeScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsSetTimeScreen::onTimeFormatChanged(QString timeFormat)
{
    timeEdit->setDisplayFormat(timeFormat);
}

void SettingsSetTimeScreen::onTimeEditReset()
{
    resetTimeEdit();
}

void SettingsSetTimeScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}

void SettingsSetTimeScreen::onSetTimeBtnClicked()
{
    settingsScreenModel->setNewTime();
    resetTimeEdit();
}

void SettingsSetTimeScreen::resetTimeEdit()
{
    timeEdit->setTime(QTime(0,0));
    timeEdit->clearFocus();
}
