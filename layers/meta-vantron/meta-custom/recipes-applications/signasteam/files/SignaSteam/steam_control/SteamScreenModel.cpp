#include "SteamScreenModel.h"

#include "SteamCanInterface.h"
#include "../settings_equipment/EquipmentCanInterface.h"

#include <QTimer>
#include <QDebug>

SteamScreenModel::SteamScreenModel(QObject *parent, SteamCanInterface *steamCanInterface, EquipmentCanInterface *equipmentCanInterface) : QObject(parent)
{
    this->steamCanInterface = steamCanInterface;
    this->equipmentCanInterface = equipmentCanInterface;

    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &SteamScreenModel::onTimeCountedDown);
}

void SteamScreenModel::onEquipmentDisconnected()
{
    if(countdownTimer->isActive()) {
        currentTime = 1;
        onTimeCountedDown();
    }
}

void SteamScreenModel::onDecreaseTempBtnClicked()
{
    if(tempScaleInF) {
        equipmentCanInterface->setTemperatureFormat((int)TempScale::FAHRENHEIT);
        currentTemp--;
    } else {
        equipmentCanInterface->setTemperatureFormat((int)TempScale::CELCIUS);
        currentTemp -= 1.8;
    }
    validateAndSendOffTemp();
}

void SteamScreenModel::onIncreaseTempBtnClicked()
{
    if(tempScaleInF) {
        equipmentCanInterface->setTemperatureFormat((int)TempScale::FAHRENHEIT);
        currentTemp++;
    } else {
        equipmentCanInterface->setTemperatureFormat((int)TempScale::CELCIUS);
        currentTemp += 1.8;
    }
    validateAndSendOffTemp();
}

void SteamScreenModel::onDecreaseTimeBtnClicked()
{
    currentTime -= 60;
    currentTime -= currentTime % 60;
    currentTime = validateTime(currentTime);

    previousStartTime = currentTime;
    sendTimeToCan(currentTime);
    emit steamTimeChanged(currentTime);
}

void SteamScreenModel::onIncreaseTimeBtnClicked()
{
    if(countdownTimer->isActive()) {
        currentTime += 120;
    } else {
        currentTime += 60;
    }
    currentTime -= currentTime % 60;
    currentTime = validateTime(currentTime);

    previousStartTime = currentTime;
    sendTimeToCan(currentTime);
    emit steamTimeChanged(currentTime);
}

void SteamScreenModel::onUserLoggedInSteamTemp(double tempFromDb)
{
    currentTemp = tempFromDb;
    if(tempScaleInF) {

        emit steamTempChanged(currentTemp);
    } else {
        emit steamTempChanged(convertTempToC(currentTemp));
    }
    sendTempToCan(currentTemp);
}

void SteamScreenModel::onUserLoggedInSteamTime(int timeFromDb)
{
    previousStartTime = timeFromDb;
    currentTime = timeFromDb;
    sendTimeToCan(currentTime);
    emit steamTimeChanged(currentTime);
}

void SteamScreenModel::onSlideToStartSwiped()
{
    previousStartTime = currentTime;
    steamCanInterface->startSteamSession();
    countdownTimer->start(1000);
}

void SteamScreenModel::onStopSessionClicked()
{
    finishSteamSession();

    //thing are messy.. the PSB sends steam time info back to signatouch..
    //since user can only set whole minutes.. this will round up to the nearest minute and
    // update the display
    if(currentTime%60!=0)
    {
        currentTime = 1 + (currentTime/60);
        currentTime = currentTime * 60;
    }
    emit steamTimeChanged(currentTime);
   // emit steamTimeChanged(1200);
}

void SteamScreenModel::onTempScaleChanged(QString tempScale)
{
    if(tempScale == appPersistence::TEMP_SCALE_F_VALUE) {
        tempScaleInF = true;
        equipmentCanInterface->setTemperatureFormat((int)TempScale::FAHRENHEIT);
        emit steamTempChanged(currentTemp);
    } else {
        tempScaleInF = false;
        equipmentCanInterface->setTemperatureFormat((int)TempScale::CELCIUS);
        emit steamTempChanged(convertTempToC(currentTemp));
    }
}

void SteamScreenModel::onUserLoggedOut()
{
    onEquipmentDisconnected();
    emit userLoggedOutSteamTemp(currentTemp);
    emit userLoggedOutSteamTime(currentTime);
}

void SteamScreenModel::onTimeCountedDown()
{
    currentTime -= 1;
    if(currentTime <= 0) {
        finishSteamSession();
        emit steamSessionTimedOut();
    }
    emit steamTimeChanged(currentTime);
}

void SteamScreenModel::validateAndSendOffTemp()
{
    currentTemp = validateTemp(currentTemp);

    sendTempToCan(currentTemp);
    if(tempScaleInF) {
        emit steamTempChanged(currentTemp);
    } else {
        emit steamTempChanged(convertTempToC(currentTemp));
    }
}

double SteamScreenModel::validateTemp(double temp)
{
    if(temp > TEMP_MAX) {
        temp = TEMP_MAX;
    } else if(temp < TEMP_MIN) {
        temp = TEMP_MIN;
    }

    return temp;
}

int SteamScreenModel::validateTime(int time)
{
    if(time > TIME_SECONDS_MAX) {
        time = TIME_SECONDS_MAX;
    } else if(time < TIME_SECONDS_MIN) {
        time = TIME_SECONDS_MIN;
    }

    return time;
}

void SteamScreenModel::sendTempToCan(double temp)
{
    steamCanInterface->changeSteamTemperature(temp);
}

void SteamScreenModel::sendTimeToCan(int time)
{
    steamCanInterface->changeSteamSessionTime(time);
}

void SteamScreenModel::finishSteamSession()
{
    currentTime = previousStartTime;
    countdownTimer->stop();
    steamCanInterface->stopSteamSession();
}

double SteamScreenModel::convertTempToC(double temp)
{
    return (temp - 32.0) / 1.8;
}
