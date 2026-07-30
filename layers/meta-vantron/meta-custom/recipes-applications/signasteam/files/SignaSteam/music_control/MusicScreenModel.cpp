#include "MusicScreenModel.h"

#include "MusicCanInterface.h"

#include <QDebug>
#include <QTimer>
#include "development/DevFunctions.h"

MusicScreenModel::MusicScreenModel(QObject *parent, MusicCanInterface *musicCanInterface)
    : QObject(parent)
{
    this->musicCanInterface = musicCanInterface;

    connect(musicCanInterface, &MusicCanInterface::getInputState, this, &MusicScreenModel::onMusicInputRecieved);
    connect(musicCanInterface, &MusicCanInterface::btAvailability, this, &MusicScreenModel::onBluetoothAvailabilityReceived);

    checkMusicInputStateTimer = new QTimer(this);
    connect(checkMusicInputStateTimer, &QTimer::timeout, this, &MusicScreenModel::handleMusicInputState);
    checkMusicInputStateTimer->start(1000);
}

void MusicScreenModel::onShowModeChecked(bool isShowModeEnabled){
    inShowMode = isShowModeEnabled;
}

void MusicScreenModel::onBluetoothAvailabilityReceived(BluetoothEquipment btEquipment){


    switch(btEquipment){
        case BluetoothEquipment::BT_EQUIPMENT_AVAILABLE:
            isBluetoothAvailable = true;
            break;
        case BluetoothEquipment::BT_EQUIPMENT_UNAVAILABLE:
            if(!inShowMode){
                isBluetoothAvailable = false;
            }else{
                isBluetoothAvailable = true;
            }
            break;
    }
    emit btEquipmentDetected(isBluetoothAvailable);
}

void MusicScreenModel::onMusicInputRecieved(QString musicInputText, QString deviceName){
    emit musicInputStateChange(musicInputText, deviceName);
}

void MusicScreenModel::onTrackSkipBackBtnClicked()
{
    musicCanInterface->btSkipBack();
}

void MusicScreenModel::onPauseBtnPressed()
{
    musicCanInterface->btPause();
}

void MusicScreenModel::onPlayBtnPressed()
{
    musicCanInterface->btPlay();
}

void MusicScreenModel::onTrackSkipForwardBtnClicked()
{
    musicCanInterface->btSkipForward();
}

void MusicScreenModel::onTrebleControlSliderChanged(int trebleValue)
{
    currentTrebleValue = trebleValue;
    musicCanInterface->changeTreble(currentTrebleValue);
}

void MusicScreenModel::onUserLoggedInTreble(int trebleValue)
{
    musicCanInterface->changeTreble(currentTrebleValue);
    currentTrebleValue = trebleValue;
    emit trebleValueChanged(currentTrebleValue);
}

void MusicScreenModel::onMidControlSliderChanged(int midValue)
{
    currentMidValue = midValue;
    musicCanInterface->changeMid(currentMidValue);
}

void MusicScreenModel::onUserLoggedInMid(int midValue)
{
    musicCanInterface->changeMid(currentMidValue);
    currentMidValue = midValue;
    emit midValueChanged(currentMidValue);
}

void MusicScreenModel::onBassControlSliderChanged(int bassValue)
{
    currentBassValue = bassValue;
    musicCanInterface->changeBass(currentBassValue);
}

void MusicScreenModel::onUserLoggedInBass(int bassValue)
{
    musicCanInterface->changeBass(currentBassValue);
    currentBassValue = bassValue;
    emit bassValueChanged(currentBassValue);
}

void MusicScreenModel::onBtToggleStateChanged(bool btState)
{
    currentBtInputState = btState;
    checkMusicInputStateTimer->start();
    emit btChanged(currentBtInputState);

}

void MusicScreenModel::onUserLoggedInBtToggle(bool btState)
{
    currentBtInputState = btState;
    checkMusicInputStateTimer->start();
    emit btChanged(currentBtInputState);
}

void MusicScreenModel::handleMusicInputState()
{
    if(currentBtInputState && !currentAuxInputState){
        musicCanInterface->changeMusicInput(static_cast<MusicInput>(1));
        checkMusicInputStateTimer->stop();
    }else if(!currentBtInputState && currentAuxInputState){
        musicCanInterface->changeMusicInput(static_cast<MusicInput>(2));
        checkMusicInputStateTimer->stop();
    }else if(!currentBtInputState && !currentAuxInputState){
        musicCanInterface->changeMusicInput(static_cast<MusicInput>(0));
        checkMusicInputStateTimer->stop();
    }
}

void MusicScreenModel::onAuxToggleStateChanged(bool auxState)
{
    currentAuxInputState = auxState;
    checkMusicInputStateTimer->start();
    emit auxChanged(currentAuxInputState);
}

void MusicScreenModel::onUserLoggedInAuxToggle(bool auxState)
{
    currentAuxInputState = auxState;
    checkMusicInputStateTimer->start();
    emit auxChanged(currentAuxInputState);
}

void MusicScreenModel::onUserLoggedOut()
{
    musicCanInterface->changeMusicInput(static_cast<MusicInput>(0));
    emit userLoggedOutTreble(currentTrebleValue);
    emit userLoggedOutMid(currentMidValue);
    emit userLoggedOutBass(currentBassValue);
    emit userLoggedOutAuxToggle(currentAuxInputState);
    emit userLoggedOutBtToggle(currentBtInputState);
}
