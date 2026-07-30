#include "HVSLRLightScreenModel.h"

#include "HVSLRLightCanInterface.h"

#include <QDebug>
#include <QTimer>
#include <QSlider>

HVSLRLightScreenModel::HVSLRLightScreenModel(QObject *parent, HVSLRLightCanInterface *HVSLRlightCanInterface) :
    QObject(parent)
{
    this->HVSLRlightCanInterface = HVSLRlightCanInterface;
    isSteamOn = false;
}

void HVSLRLightScreenModel::onSteamLightOn(bool hasSteamStarted){
    isSteamOn = hasSteamStarted;
    if(!isUserLoggedOut){ //Required to not get error ASSERT "index out of range"
        if(isSteamOn){
            HVSLRlightCanInterface->setSteamIsOn(isSteamOn);
            emit lightChanged(steamLightButtonClicked);
            HVSLRlightCanInterface->setLightIntensity(currentSteamLightBrightnessValue);
            emit lightBrightnessChanged(currentSteamLightBrightnessValue);
            emit brightnessSliderLightToggle(true);
            if(currentLightToggleStateOn)
            {
                if(steamLightButtonClicked == Light::WHITE)
                {
                    onWhiteWheelChange(getSteamRGB());
                }else if (steamLightButtonClicked == Light::COLOR)
                {
                    onColorWheelChange(getSteamRGB());
                } else
                {
                    HVSLRlightCanInterface->changeLight(steamLightButtonClicked);
                }
            }
        }else{
             HVSLRlightCanInterface->setSteamIsOn(isSteamOn);
            emit lightChanged(lightButtonClicked);
           HVSLRlightCanInterface->setLightIntensity(currentLightBrightnessValue);
            emit lightBrightnessChanged(currentLightBrightnessValue);
            emit brightnessSliderLightToggle(true);
            if(currentLightToggleStateOn)
            {
                if(lightButtonClicked == Light::WHITE)
                {
                    onWhiteWheelChange(getRGB());
                }else if (lightButtonClicked == Light::COLOR)
                {
                    onColorWheelChange(getRGB());
                } else
                {
                    HVSLRlightCanInterface->changeLight(lightButtonClicked);
                }
            }
        }
    }
}

void HVSLRLightScreenModel::onColorLightBtnClicked(int lightButtonClickedAsInt)
{


    if(isSteamOn){
        steamLightButtonClicked = static_cast<Light>(lightButtonClickedAsInt);        
        HVSLRlightCanInterface->changeLight(steamLightButtonClicked);
        emit lightChanged(steamLightButtonClicked);
    }else{
        lightButtonClicked = static_cast<Light>(lightButtonClickedAsInt);
        HVSLRlightCanInterface->changeLight(lightButtonClicked);
        emit lightChanged(lightButtonClicked);
    }
}

void HVSLRLightScreenModel::onUserLoggedInLightColor(int lightButtonClickedAsInt)
{

    lightButtonClicked = static_cast<Light>(lightButtonClickedAsInt);

    emit lightChanged(lightButtonClicked);

    if(!currentLightToggleStateOn){

        HVSLRlightCanInterface->turnLightOff();
    }else{
        HVSLRlightCanInterface->changeLight(lightButtonClicked);
        // Can't seem to replicate bug QLIST "index out of range" anymore with signal below. I think it is fixed after multiple tests and edits but leaving comment just in case.
        emit lightChanged(lightButtonClicked);
    }
}

void HVSLRLightScreenModel::onUserLoggedInPreset1Color(int presetColor)
{
    emit preset1ColorChanged(presetColor);
}

void HVSLRLightScreenModel::onUserLoggedInPreset2Color(int presetColor)
{
    emit preset2ColorChanged(presetColor);
}

void HVSLRLightScreenModel::onUserLoggedInPreset3Color(int presetColor)
{
    emit preset3ColorChanged(presetColor);
}

void HVSLRLightScreenModel::onUserLoggedInPreset1White(int presetColor)
{
    emit preset1WhiteChanged(presetColor);
}

void HVSLRLightScreenModel::onUserLoggedInPreset2White(int presetColor)
{
    emit preset2WhiteChanged(presetColor);
}

void HVSLRLightScreenModel::onUserLoggedInPreset3White(int presetColor)
{
    emit preset3WhiteChanged(presetColor);
}




void HVSLRLightScreenModel::onUserLoggedInSteamLightColor(int steamLightValue){
    isUserLoggedOut = false;
    steamLightButtonClicked = static_cast<Light>(steamLightValue);
}

void HVSLRLightScreenModel::onUserLoggedInSteamLightBrightness(int steamBrightnessValue){
    currentSteamLightBrightnessValue = steamBrightnessValue;
}

void HVSLRLightScreenModel::onLightIntensitySliderChanged(int lightIntensity)
{
    if(isSteamOn){
        currentSteamLightBrightnessValue = lightIntensity;
        HVSLRlightCanInterface->changeSteamLightIntensity(currentSteamLightBrightnessValue);
        emit lightBrightnessChanged(currentSteamLightBrightnessValue);
        emit brightnessSliderLightToggle(true);
    }else{
        currentLightBrightnessValue = lightIntensity;
        HVSLRlightCanInterface->changeLightIntensity(currentLightBrightnessValue);
        emit lightBrightnessChanged(currentLightBrightnessValue);
        emit brightnessSliderLightToggle(true);
    }
}

void HVSLRLightScreenModel::onUserLoggedInLightBrightness(int brightnessValue)
{
    currentLightBrightnessValue = brightnessValue;
    HVSLRlightCanInterface->changeLightIntensity(currentLightBrightnessValue);
    emit lightBrightnessChanged(currentLightBrightnessValue);
}

void HVSLRLightScreenModel::onLightToggleStateChanged(bool isChecked)
{
    currentLightToggleStateOn = isChecked;

    if(!isChecked){
        HVSLRlightCanInterface->turnLightOff();
        currentLightToggleStateOn = false;
    }else{
        HVSLRlightCanInterface->turnLightOn();
        emit lightChanged(lightButtonClicked);
        HVSLRlightCanInterface->changeLightIntensity(currentLightBrightnessValue);
    }
}

void HVSLRLightScreenModel::onUserLoggedInLightToggle(bool lightBtnStateValue){
    currentLightToggleStateOn = lightBtnStateValue;

    if(currentLightToggleStateOn)
    {
        HVSLRlightCanInterface->turnLightOn();
    }

    emit lightToggleChanged(currentLightToggleStateOn);
}

void HVSLRLightScreenModel::onUserLoggedInLightMode(int lightMood)
{

    steamLightButtonClicked = static_cast<Light>(lightMood);
    HVSLRlightCanInterface->changeLight(steamLightButtonClicked);
    emit lightChanged(steamLightButtonClicked);
}

void HVSLRLightScreenModel::onUserLoggedInRGB(int rgb)
{
    QRgb pixel;
    pixel = static_cast<QRgb>(rgb);

    int alpha = qAlpha(pixel);
    int red = qRed(pixel);
    int green = qGreen(pixel);
    int blue = qBlue(pixel);


    if(currentLightToggleStateOn && !isSteamOn && static_cast<Light>(HVSLRLightScreenModel::getColorMode()) != Light::WHITE)
    {
        HVSLRlightCanInterface->setRGBTurnOn(red,green,blue);
    }
    else
    {
        HVSLRlightCanInterface->setRGB(red,green,blue);
    }
}

void HVSLRLightScreenModel::onUserLoggedInSteamRGB(int rgb)
{
    QRgb pixel;
    pixel = static_cast<QRgb>(rgb);

    int alpha = qAlpha(pixel);
    int red = qRed(pixel);
    int green = qGreen(pixel);
    int blue = qBlue(pixel);


    if(currentLightToggleStateOn && isSteamOn && static_cast<Light>(HVSLRLightScreenModel::getColorMode()) != Light::WHITE)
    {
        HVSLRlightCanInterface->setSteamRGBTurnOn(red,green,blue);
    }
    else
    {
        HVSLRlightCanInterface->setSteamRGB(red,green,blue);
    }

}

void HVSLRLightScreenModel::onUserLoggedOut()
{
    isUserLoggedOut = true;
    emit userLoggedOutLightBrightness(currentLightBrightnessValue);
    emit userLoggedOutLightToggle(currentLightToggleStateOn);
    HVSLRlightCanInterface->turnLightOff();
    emit userLoggedOutLightColor( (int) lightButtonClicked);
    emit userLoggedOutSteamLightColor((int) steamLightButtonClicked);
    emit userLoggedOutSteamBrightness(currentSteamLightBrightnessValue);
    emit userLoggedOutColorMode( static_cast<int>(getColorMode()));
    emit userLoggedOutRGB( static_cast<int>(getRGB()));
    emit userLoggedOutSteamRGB(static_cast<int>(getSteamRGB()));
}

void HVSLRLightScreenModel::onColorWheelChange(QRgb pixel)
{
    int alpha = qAlpha(pixel);
    int red = qRed(pixel);
    int green = qGreen(pixel);
    int blue = qBlue(pixel);


    //qDebug() << "red = " << red << "green = " << green << "blue = " << blue << "alpha =" << alpha;
    if(isSteamOn)
    {
        HVSLRlightCanInterface->setSteamRGBTurnOn(red,green,blue);
    }
    else
    {
        HVSLRlightCanInterface->setRGBTurnOn(red,green,blue);
    }
}

void HVSLRLightScreenModel::onWhiteWheelChange(QRgb pixel)
{
    int red = qRed(pixel);
    int green = qGreen(pixel);
    int blue = qBlue(pixel);


    if(isSteamOn)
    {
        HVSLRlightCanInterface->setSteamWhiteRGBTurnOn(red,green,blue);
    }else
    {
        HVSLRlightCanInterface->setWhiteRGBTurnOn(red,green,blue);
    }

}

unsigned int HVSLRLightScreenModel::getRGB()
{
    return HVSLRlightCanInterface->getRGB();
}

unsigned int HVSLRLightScreenModel::getSteamRGB()
{
    return HVSLRlightCanInterface->getSteamRGB();
}

void HVSLRLightScreenModel::setColorMode(int mode)
{
    currentLightMode = mode;
}

unsigned int HVSLRLightScreenModel::getColorMode()
{
    return currentLightMode;
}


