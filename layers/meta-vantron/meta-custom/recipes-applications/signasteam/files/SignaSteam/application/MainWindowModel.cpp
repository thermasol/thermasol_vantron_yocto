#include "MainWindowModel.h"
extern "C" {
    #include <thermacan.h>
}

#include "settings_equipment/EquipmentCanInterface.h"
#include "application/AppConstants.h"

#include <QDateTime>
#include <QTimer>

#include <QDebug>
#include "development/DevFunctions.h"

MainWindowModel::MainWindowModel(QObject *parent, EquipmentCanInterface *equipmentCanInterface) :
    QObject(parent)
{   
    this->equipmentCanInterface = equipmentCanInterface;

    currentNavBarState.insert(NavBarButton::SHOWER, NavBarButtonState::ABSENT);
    currentNavBarState.insert(NavBarButton::STEAM, NavBarButtonState::ABSENT);
    currentNavBarState.insert(NavBarButton::MUSIC, NavBarButtonState::ABSENT);
    currentNavBarState.insert(NavBarButton::LIGHT, NavBarButtonState::ABSENT);

    connect(this->equipmentCanInterface, &EquipmentCanInterface::canDevicesStateChanged, this, &MainWindowModel::onDevicesStateChanged);
    QTimer *clockTimeUpdater = new QTimer(this);
    connect(clockTimeUpdater, &QTimer::timeout, this, &MainWindowModel::onTimeChanged);
    clockTimeUpdater->start(60000);
    HVSLR = false;
}

void MainWindowModel::onVolumeBarSliderChanged(int volumeBarHandleValue)
{
    currentVolumeValue = volumeBarHandleValue;
    equipmentCanInterface->changeVolume(currentVolumeValue);
    emit volumeSliderMuteToggleChanged(true);
}

void MainWindowModel::onUserLoggedInVolume(int volumeValue)
{
    currentVolumeValue = volumeValue;
    equipmentCanInterface->changeVolume(currentVolumeValue);
    emit volumeValueChanged(currentVolumeValue);

}

void MainWindowModel::onMuteToggleStateChanged(bool isChecked){

    currentMuteBtnStateValue = isChecked;

    if(!isChecked){
        equipmentCanInterface->changeVolume(0);
    }else{
        equipmentCanInterface->changeVolume(currentVolumeValue);
    }

}

void MainWindowModel::onUserLoggedInMuteToggle(bool muteBtnStateValue)
{
    currentMuteBtnStateValue = muteBtnStateValue;
    if(muteBtnStateValue == true)
    {
        equipmentCanInterface->mute();
    }
    else
    {
        equipmentCanInterface->unmute();
    }
    emit muteToggleChanged(currentMuteBtnStateValue);
}

void MainWindowModel::onTimeChanged()
{
    QString clockTime = QDateTime::currentDateTime().toString(timeFormat);
    emit clockTimeChanged(clockTime);
}

void MainWindowModel::onTimeFormatChanged(QString timeFormat)
{
    this->timeFormat = timeFormat;
    onTimeChanged();
}

bool MainWindowModel::isHVSLR()
{
    return HVSLR;
}

void MainWindowModel::onDevicesStateChanged(QMap<CanDevice, CanDeviceState> canDevices)
{   
    QMap<CanDevice, CanDeviceState>::iterator iterator;
    for (iterator = canDevices.begin(); iterator != canDevices.end(); ++iterator) {
        switch(iterator.key()) {
            case CanDevice::DIGITAL_SHOWER:
                if(iterator.value() == CanDeviceState::PRESENT) {
                    currentNavBarState[NavBarButton::SHOWER] = NavBarButtonState::PRESENT;
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                    currentNavBarState[NavBarButton::SHOWER] = NavBarButtonState::ABSENT;
                }
                break;
            case CanDevice::STEAM_GENERATOR:
                if(iterator.value() == CanDeviceState::PRESENT) {
                    currentNavBarState[NavBarButton::STEAM] = NavBarButtonState::PRESENT;
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                    currentNavBarState[NavBarButton::STEAM] = NavBarButtonState::ABSENT;
                }
                break;
            case CanDevice::SLSR:
                if(iterator.value() == CanDeviceState::PRESENT) {
                    HVSLR = false;
                    currentNavBarState[NavBarButton::MUSIC] = NavBarButtonState::PRESENT;
                    currentNavBarState[NavBarButton::LIGHT] = NavBarButtonState::PRESENT;
                } else if(iterator.value() == CanDeviceState::ABSENT) {
                   // currentNavBarState[NavBarButton::MUSIC] = NavBarButtonState::ABSENT;
                  //  currentNavBarState[NavBarButton::LIGHT] = NavBarButtonState::ABSENT;
                }
                break;
           case CanDevice::HVSLSR:
                if(iterator.value() == CanDeviceState::PRESENT) {
                    HVSLR = true;
                    currentNavBarState[NavBarButton::MUSIC] = NavBarButtonState::PRESENT;
                    currentNavBarState[NavBarButton::LIGHT] = NavBarButtonState::PRESENT;
                }else if (iterator.value() == CanDeviceState::ABSENT) {
                  //  currentNavBarState[NavBarButton::MUSIC] = NavBarButtonState::ABSENT;
                  //  currentNavBarState[NavBarButton::LIGHT] = NavBarButtonState::ABSENT;
                }
                break;
        }
    }

    emit navBarChanged(currentNavBarState);
}

void MainWindowModel::onEzsTurnOff()
{
    equipmentCanInterface->turnOffEzs();
}

void MainWindowModel::onUserLoggedOut()
{
    emit userLoggedOutMuteToggle(currentMuteBtnStateValue);
    emit userLoggedOutVolume(currentVolumeValue);
}
