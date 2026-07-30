#include "MusicScreen.h"
#include "ui_MusicScreen-landscape.h"
#include "ui_MusicScreen-portrait.h"

#include "application/AppConstants.h"
#include "MusicScreenModel.h"

#include <QSettings>

#include <QDebug>
#include "development/DevFunctions.h"

MusicScreen::MusicScreen(QWidget *parent, MusicScreenModel *musicScreenModel) :
    QWidget(parent)
{

    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::MusicScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::MusicScreenPortrait;
        uiPortrait->setupUi(this);
    }

    trackSkipBackBtn = findChild<QToolButton*>("trackSkipBackBtn");
    connect(trackSkipBackBtn, &QToolButton::clicked, musicScreenModel, &MusicScreenModel::onTrackSkipBackBtnClicked);

    pauseBtn = findChild<QToolButton*>("pauseBtn");
    connect(pauseBtn, &QToolButton::clicked, musicScreenModel, &MusicScreenModel::onPauseBtnPressed);

    playBtn = findChild<QToolButton*>("playBtn");
    connect(playBtn, &QToolButton::clicked, musicScreenModel, &MusicScreenModel::onPlayBtnPressed);

    trackSkipForwardBtn = findChild<QToolButton*>("trackSkipForwardBtn");
    connect(trackSkipForwardBtn, &QToolButton::clicked, musicScreenModel, &MusicScreenModel::onTrackSkipForwardBtnClicked);

    connect(musicScreenModel, &MusicScreenModel::btEquipmentDetected, this, &MusicScreen::onBtEquipmentDetected);

    bluetoothInputBtn = findChild<QToolButton*>("bluetoothInputBtn");
    connect(bluetoothInputBtn, &QToolButton::toggled, musicScreenModel, &MusicScreenModel::onBtToggleStateChanged);
    connect(musicScreenModel, &MusicScreenModel::btChanged, this, &MusicScreen::onBtChanged);

    auxInputBtn = findChild<QToolButton*>("auxInputBtn");
    connect(auxInputBtn, &QToolButton::toggled, musicScreenModel, &MusicScreenModel::onAuxToggleStateChanged);
    connect(musicScreenModel, &MusicScreenModel::auxChanged, this, &MusicScreen::onAuxChanged);

    musicInputLabel = findChild<QLabel*>("musicInputLabel");
    connect(musicScreenModel, &MusicScreenModel::musicInputStateChange, this, &MusicScreen::onMusicInputStateChanged);
    musicInputLabel->setFont(fontStyle::getQuicksandBoldWithSize(MUSIC_INPUT_LABEL_FONT));
    musicInputLabel->adjustSize();

    deviceInputLabel = findChild<QLabel*>("deviceInputLabel");
    deviceInputLabel->setFont(fontStyle::getQuicksandBoldWithSize(MUSIC_INPUT_LABEL_FONT));
    deviceInputLabel->adjustSize();

    trebleControlSlider = findChild<EqLineQSlider*>("trebleControlSlider");
    trebleControlSlider->setStyleSheet(QSLIDER_STYLESHEET);
    connect(musicScreenModel, &MusicScreenModel::trebleValueChanged, this, &MusicScreen::onTrebleChanged);
    connect(trebleControlSlider, &EqLineQSlider::valueChanged, musicScreenModel, &MusicScreenModel::onTrebleControlSliderChanged);

    midControlSlider = findChild<EqLineQSlider*>("midControlSlider");
    midControlSlider->setStyleSheet(QSLIDER_STYLESHEET);
    connect(musicScreenModel, &MusicScreenModel::midValueChanged, this, &MusicScreen::onMidChanged);
    connect(midControlSlider, &EqLineQSlider::valueChanged, musicScreenModel, &MusicScreenModel::onMidControlSliderChanged);

    bassControlSlider = findChild<EqLineQSlider*>("bassControlSlider");
    bassControlSlider->setStyleSheet(QSLIDER_STYLESHEET);
    connect(musicScreenModel, &MusicScreenModel::bassValueChanged, this, &MusicScreen::onBassChanged);
    connect(bassControlSlider, &EqLineQSlider::valueChanged, musicScreenModel, &MusicScreenModel::onBassControlSliderChanged);

}

MusicScreen::~MusicScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void MusicScreen::onMusicInputStateChanged(QString musicInputText, QString deviceName)
{
    if(!isAuxAvailable && !isBtAvailable){
        musicInputLabel->setText("No Music Input");
        deviceInputLabel->setText("");
    }else if(isAuxAvailable){
        musicInputLabel->setText("AUX");
        deviceInputLabel->setText("");
    }else if(isBtAvailable){
        musicInputLabel->setText(musicInputText);
        deviceInputLabel->setText(deviceName);
    }

}

void MusicScreen::onBtEquipmentDetected(bool isBtEquiped){
    if(!isBtEquiped){        
        musicInputLabel->hide();
        trackSkipBackBtn->hide();
        playBtn->hide();
        pauseBtn->hide();
        trackSkipForwardBtn->hide();
        bluetoothInputBtn->hide();
    }else{
        musicInputLabel->show();
        trackSkipBackBtn->show();
        playBtn->show();
        pauseBtn->show();
        trackSkipForwardBtn->show();
        bluetoothInputBtn->show();
    }
}

void MusicScreen::onBtChanged(bool isBtOn){

    isBtAvailable = isBtOn;

    if(isBtOn){
        auxInputBtn->setChecked(false);
    }
    bluetoothInputBtn->setChecked(isBtOn);
}

void MusicScreen::onAuxChanged(bool isAuxOn){

    isAuxAvailable = isAuxOn;

    if(isAuxOn){
        bluetoothInputBtn->setChecked(false);
    }
    auxInputBtn->setChecked(isAuxOn);
}


void MusicScreen::onTrebleChanged(int trebleValue)
{
    trebleControlSlider->setValue(trebleValue);
}

void MusicScreen::onMidChanged(int midValue)
{
    midControlSlider->setValue(midValue);
}

void MusicScreen::onBassChanged(int bassValue)
{
    bassControlSlider->setValue(bassValue);
}



