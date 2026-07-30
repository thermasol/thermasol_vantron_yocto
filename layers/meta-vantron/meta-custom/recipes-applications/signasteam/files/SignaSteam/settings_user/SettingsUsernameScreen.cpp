#include "SettingsUsernameScreen.h"
#include "ui_SettingsUsernameScreen-landscape.h"
#include "ui_SettingsUsernameScreen-portrait.h"

#include "settings/SettingsScreenModel.h"

#include <QDebug>

SettingsUsernameScreen::SettingsUsernameScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel) :
    QWidget(parent)
{
    this->settingsScreenModel = settingsScreenModel;

    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsUsernameScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsUsernameScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsUsernameScreen::onBackButtonClicked);

    usernameText = findChild<QLabel*>("usernameText");
    usernameText->setFont(fontStyle::getQuicksandBoldWithSize(USERNAME_FONT_SIZE));
    connect(settingsScreenModel, &SettingsScreenModel::usernameChanged, this, &SettingsUsernameScreen::onUsernameChanged);

    usernameTextClickArea = findChild<QToolButton*>("usernameTextClickArea");
    connect(usernameTextClickArea, &QToolButton::clicked, this, &SettingsUsernameScreen::onUsernameClicked);

    QToolButton* userIconSelectIncreaseBtn = findChild<QToolButton*>("userIconSelectIncreaseBtn");
    connect(userIconSelectIncreaseBtn, &QToolButton::clicked, settingsScreenModel, &SettingsScreenModel::onUserIconUpBtnClicked);

    QToolButton* userIconSelectDecreaseBtn = findChild<QToolButton*>("userIconSelectDecreaseBtn");
    connect(userIconSelectDecreaseBtn, &QToolButton::clicked, settingsScreenModel, &SettingsScreenModel::onUserIconDownBtnClicked);

    userIcon = findChild<QLabel*>("userIcon");
    connect(settingsScreenModel, &SettingsScreenModel::userIconChanged, this, &SettingsUsernameScreen::onUserIconChanged);

    QToolButton* saveBtn = findChild<QToolButton*>("saveBtn");
    connect(saveBtn, &QToolButton::clicked, this, &SettingsUsernameScreen::onSaveBtnClicked);
}

SettingsUsernameScreen::~SettingsUsernameScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsUsernameScreen::onUserIconChanged(QString iconPath)
{
    userIcon->setPixmap(QPixmap(iconPath));
}

void SettingsUsernameScreen::onUsernameChanged(QString username)
{
    usernameText->setText(username);
}

void SettingsUsernameScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}

void SettingsUsernameScreen::onUsernameClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::KEYBOARD);
}

void SettingsUsernameScreen::onSaveBtnClicked()
{
    settingsScreenModel->saveUsernameAndIcon();
}
