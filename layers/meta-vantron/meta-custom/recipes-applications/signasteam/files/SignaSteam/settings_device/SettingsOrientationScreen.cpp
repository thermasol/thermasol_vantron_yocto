#include "SettingsOrientationScreen.h"
#include "ui_SettingsOrientationScreen-landscape.h"
#include "ui_SettingsOrientationScreen-portrait.h"

#include "settings/SettingsScreenModel.h"

#include <QDebug>

SettingsOrientationScreen::SettingsOrientationScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel) : QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsOrientationScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsOrientationScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton* backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsOrientationScreen::onBackButtonClicked);

    QButtonGroup* orientationButtonGroup = findChild<QButtonGroup*>("orientationButtonGroup");
    connect(orientationButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &SettingsOrientationScreen::onOrientationBtnClicked);
    connect(this, &SettingsOrientationScreen::orientationChanged, settingsScreenModel, &SettingsScreenModel::onOrientationChanged);

    QToolButton* landscapeButton = findChild<QToolButton*>("landscapeBtn");
    orientationButtonGroup->setId(landscapeButton, static_cast<int>(Orientation::LANDSCAPE));

    QToolButton* portraitButton = findChild<QToolButton*>("portraitBtn");
    orientationButtonGroup->setId(portraitButton, static_cast<int>(Orientation::PORTRAIT));

    landscapeBtnLabel = findChild<QToolButton*>("landscapeBtnLabel");
    portraitBtnLabel = findChild<QToolButton*>("portraitBtnLabel");

    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        landscapeButton->setChecked(true);
        landscapeBtnLabel->setChecked(true);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        portraitButton->setChecked(true);
        portraitBtnLabel->setChecked(true);
    }
}

SettingsOrientationScreen::~SettingsOrientationScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsOrientationScreen::onOrientationBtnClicked(int orientationBtnClickedAsInt)
{
    Orientation orientationClickedId = static_cast<Orientation>(orientationBtnClickedAsInt);

    switch (orientationClickedId) {
        case Orientation::LANDSCAPE:
            landscapeBtnLabel->setChecked(true);
            break;
        case Orientation::PORTRAIT:
            portraitBtnLabel->setChecked(true);
            break;
    }

    emit orientationChanged(orientationClickedId);
}

void SettingsOrientationScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::GENERAL);
}
