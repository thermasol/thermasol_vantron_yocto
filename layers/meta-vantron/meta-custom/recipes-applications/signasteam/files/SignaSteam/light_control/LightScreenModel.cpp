#include "LightScreenModel.h"

#include "LightCanInterface.h"
#include "../util/lighthelper.h"

#include <QDebug>
#include <QTimer>

LightScreenModel::LightScreenModel(QObject *parent, LightCanInterface *lightCanInterface) :
    QObject(parent)
{
    this->lightCanInterface = lightCanInterface;
    isSteamOn = false;
}

void LightScreenModel::onSteamLightOn(bool hasSteamStarted){
    isSteamOn = hasSteamStarted;
    if(!isUserLoggedOut){ //Required to not get error ASSERT "index out of range"
        if(isSteamOn){
            emit lightChanged(steam->getLightMode());
        }else{
            emit lightChanged(normal->getLightMode());
        }
        emit updateLightHardware();
    }
}

void LightScreenModel::onColorLightBtnClicked(int lightButtonClickedAsInt)
{
    LightMode LightButtonClicked = static_cast<LightMode>(lightButtonClickedAsInt);
    LightHelper *currentLightMode;

    if(isSteamOn)
        currentLightMode = steam;
    else
        currentLightMode = normal;

    //if the button has already been pressed.. just ignore
    if((currentLightMode->getLightMode() == LightButtonClicked) && currentLightMode->getLightOn())
    {
        return;
    }

   //if light is not on, let's turn it on
    if(!currentLightMode->getLightOn())
    {
        currentLightMode->turnlightOn();
    }

    currentLightMode->setLightMode(LightButtonClicked);


    emit updateLightHardware();
    emit lightChanged(LightButtonClicked);
}

void LightScreenModel::onUserLoggedInLightColor(int lightButtonClickedAsInt)
{

      LightMode LightButtonClicked = static_cast<LightMode>(lightButtonClickedAsInt);

      normal->setLightMode(LightButtonClicked);
      emit lightChanged(LightButtonClicked);
}

void LightScreenModel::onUserLoggedInSteamLightColor(int steamLightValue){

    LightMode LightButtonClicked = static_cast<LightMode>(steamLightValue);

    steam->setLightMode(LightButtonClicked);

    isUserLoggedOut = false;
}

void LightScreenModel::onUserLoggedInSteamLightBrightness(int steamBrightnessValue){
    steam->setIntensity(steamBrightnessValue);
}

void LightScreenModel::onLightIntensitySliderChanged(int lightIntensity)
{

    LightHelper *currentLightMode;

    if(isSteamOn)
        currentLightMode = steam;
    else
        currentLightMode = normal;

    if(currentLightMode->getIntensity() != lightIntensity)
    {
        currentLightMode->setIntensity(lightIntensity);

        //We want to turn the light on if the Intensity slider is changed
        currentLightMode->turnlightOn();
        emit updateLightHardware();
    }
}

void LightScreenModel::onUserLoggedInLightBrightness(int brightnessValue)
{

    normal->setIntensity(brightnessValue);

}

void LightScreenModel::onLightToggleStateChanged(bool isChecked)
{

    LightHelper *currentLightMode;

    currentLightToggleStateOn = isChecked;

    if(isSteamOn)
        currentLightMode = steam;
    else
        currentLightMode = normal;

    if(!isChecked){
        currentLightMode->turnlightOff();
        emit updateLightHardware();
    }else{
        currentLightMode->turnlightOn();
        emit updateLightHardware();
    }
}

void LightScreenModel::onUserLoggedInLightToggle(bool lightBtnStateValue){
    
    if(lightBtnStateValue)
    {
        normal->turnlightOn();
    }
    else
    {
        normal->turnlightOff();
    }

    emit updateLightHardware();

}


void LightScreenModel::onUserLoggedOut()
{

    isUserLoggedOut = true;
    emit userLoggedOutLightBrightness(normal->getIntensity());
    emit userLoggedOutLightToggle(normal->getLightOn());
    emit userLoggedOutLightColor(static_cast<int>(normal->getLightMode()));
    emit userLoggedOutSteamLightColor(static_cast<int>(steam->getLightMode()));
    emit userLoggedOutSteamBrightness(steam->getIntensity());

    lightCanInterface->turnLightOff();
}

void LightScreenModel::setLightHelper(LightHelper& normalLightHelper, LightHelper& steamLightHelper)
{
    normal = &normalLightHelper;
    steam = &steamLightHelper;

}

void LightScreenModel::updateSLSR()
{
    LightHelper *currnetLightSettings;


    if(isSteamOn)
        currnetLightSettings = steam;
    else
        currnetLightSettings = normal;

    emit lightBrightnessChanged(currnetLightSettings->getIntensity());


    if(currnetLightSettings->getLightOn())
    {
        lightCanInterface->turnLightOn();
        lightCanInterface->changeLightIntensity(currnetLightSettings->getIntensity());

        lightCanInterface->changeLight(currnetLightSettings->getLightMode());
        emit lightToggleChanged(true);
    }
    else
    {
        lightCanInterface->turnLightOff();
        emit lightToggleChanged(false);
    }




}
