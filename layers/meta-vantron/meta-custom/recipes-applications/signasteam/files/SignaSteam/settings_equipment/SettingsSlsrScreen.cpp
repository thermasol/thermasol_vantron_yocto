#include "SettingsSlsrScreen.h"
#include "ui_SettingsSlsrScreen-landscape.h"
#include "ui_SettingsSlsrScreen-portrait.h"

#include "../settings/SettingsScreenModel.h"
#include "BluetoothCanInterface.h"
#include "EquipmentCanInterface.h"

#include <QSettings>

#include <QDebug>

SettingsSlsrScreen::SettingsSlsrScreen(QWidget *parent, SettingsScreenModel *settingsScreenModel,
                                       BluetoothCanInterface *bluetoothCanInterface, EquipmentCanInterface *equipmentCanInterface) :
    QWidget(parent)
{
   this->settingsScreenModel = settingsScreenModel;
    this->bluetoothCanInterface = bluetoothCanInterface;

    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SettingsSlsrScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SettingsSlsrScreenPortrait;
        uiPortrait->setupUi(this);
    }

    QToolButton *backButton = findChild<QToolButton*>("backButton");
    connect(backButton, &QToolButton::clicked, this, &SettingsSlsrScreen::onBackButtonClicked);

    voiceVolumeIcon = findChild<QLabel*>("voiceVolumeIcon");
    voiceSectionLabel = findChild<QLabel*>("voiceSectionLabel");



    voiceVolumeBarSlider = findChild<QSlider*>("voiceVolumeBarSlider");
    connect(voiceVolumeBarSlider, &QSlider::valueChanged, settingsScreenModel, &SettingsScreenModel::onVoiceVolumeSliderChanged);
    connect(settingsScreenModel, &SettingsScreenModel::voiceVolumeChanged, this, &SettingsSlsrScreen::onVoiceVolumeChanged);
    voiceVolumeBarSlider->setValue(DEFAULT_VOICE_VALUE);
    voiceVolumeBarSlider->setStyleSheet(barQSliderStyleSheet::HORIZONTAL_BAR_QSLIDERS);

    voiceOnOffBtn = findChild<QToolButton*>("voiceOnOffBtn");
    connect(voiceOnOffBtn, &QToolButton::toggled, settingsScreenModel, &SettingsScreenModel::onVoiceToggleStateChanged);
    connect(settingsScreenModel, &SettingsScreenModel::voiceToggleChanged, this, &SettingsSlsrScreen::onVoiceToggleChanged);
    connect(settingsScreenModel, &SettingsScreenModel::voiceVolumeSliderVoiceToggle, this, &SettingsSlsrScreen::onVoiceVolumeSliderVoiceToggleChanged);

    if(equipmentCanInterface->getVoiceChipSLSR() == VoiceChip::NOVOICECHIP)
    {

        if(!voiceVolumeIcon)
        {
            voiceVolumeIcon = findChild<QLabel*>("VolumeIcon");
        }
        voiceVolumeIcon->hide();
        voiceOnOffBtn->hide();
        voiceVolumeBarSlider->hide();
        voiceSectionLabel->hide();


    }

}

SettingsSlsrScreen::~SettingsSlsrScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SettingsSlsrScreen::onVoiceVolumeChanged(int voiceVolumeValue)
{
    voiceVolumeBarSlider->setValue(voiceVolumeValue);
}

void SettingsSlsrScreen::onVoiceToggleChanged(bool voiceToggleState)
{
    voiceOnOffBtn->setChecked(voiceToggleState);
}

void SettingsSlsrScreen::onVoiceVolumeSliderVoiceToggleChanged(bool sliderHasMoved)
{
    if(sliderHasMoved){
        voiceOnOffBtn->setChecked(true);
    }
}

void SettingsSlsrScreen::onBackButtonClicked()
{
    emit settingsScreenChanged(SettingsScreenAll::EQUIPMENT);
}

