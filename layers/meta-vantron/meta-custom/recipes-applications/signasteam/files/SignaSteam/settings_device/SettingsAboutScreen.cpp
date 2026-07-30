#include "SettingsAboutScreen.h"
#include "ui_SettingsAboutScreen-landscape.h"
#include "ui_SettingsAboutScreen-portrait.h"

#include <QSettings>
#include <QPushButton>
#include <QDebug>

SettingsAboutScreen::SettingsAboutScreen(QWidget *parent) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsAboutScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsAboutScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsAboutScreen::onBackButtonClicked);

    QLabel* showModeLabel = findChild<QLabel*>("showModeLabel");
    showModeLabel->setFont(fontStyle::getQuicksandBoldWithSize(ABOUT_SETTINGS_LABEL_FONT_SIZE));
    showModeLabel-> adjustSize();

    QLabel* emailLabel = findChild<QLabel*>("emailLabel");
    emailLabel->setFont(fontStyle::getQuicksandBoldWithSize(ABOUT_SETTINGS_LABEL_FONT_SIZE));
    emailLabel-> adjustSize();

    QLabel* firmwareLabel = findChild<QLabel*>("firmwareLabel");
    firmwareLabel->setFont(fontStyle::getQuicksandBoldWithSize(ABOUT_SETTINGS_LABEL_FONT_SIZE));
    firmwareLabel-> adjustSize();


    QLabel* firmwareValue = findChild<QLabel*>("firmwareValue");
    firmwareValue->setFont(fontStyle::getQuicksandBoldWithSize(ABOUT_SETTINGS_SUBLABEL_FONT_SIZE));
    firmwareValue->setText(firmware::FIRMWARE_VERSION);

    QLabel* emailValue = findChild<QLabel*>("emailValue");
    emailValue->setFont(fontStyle::getQuicksandBoldWithSize(ABOUT_SETTINGS_SUBLABEL_FONT_SIZE));
    emailValue->adjustSize();

    QToolButton* showModeBtn = findChild<QToolButton*>("showModeBtn");
    connect(showModeBtn, &QToolButton::toggled, this, &SettingsAboutScreen::showModeBtnClicked);
}

SettingsAboutScreen::~SettingsAboutScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsAboutScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}
