#ifndef HVSLRLIGHTSCREENMODEL_H
#define HVSLRLIGHTSCREENMODEL_H

#include "application/AppConstants.h"

#include <QObject>
#include <QColor>

class HVSLRLightCanInterface;

class HVSLRLightScreenModel : public QObject
{
    Q_OBJECT

public:
    explicit HVSLRLightScreenModel(QObject *parent = nullptr, HVSLRLightCanInterface *HVSLRlightCanInterface = nullptr);
    unsigned int getRGB();
    unsigned int getSteamRGB();
    unsigned int getColorMode();
    void setColorMode(int);

signals:
    void lightChanged(Light);
    void preset1ColorChanged(int);
    void preset2ColorChanged(int);
    void preset3ColorChanged(int);
    void preset1WhiteChanged(int);
    void preset2WhiteChanged(int);
    void preset3WhiteChanged(int);
    void lightBrightnessChanged(int);
    void lightToggleChanged(bool);
    void userLoggedOutLightBrightness(int);
    void userLoggedOutLightToggle(bool);
    void brightnessSliderLightToggle(bool);
    void userLoggedOutLightColor(int);
    void userLoggedOutSteamLightColor(int);
    void userLoggedOutSteamBrightness(int);
    void userLoggedOutColorMode(int);
    void userLoggedOutRGB(int);
    void userLoggedOutSteamRGB(int);
    void checkedState(bool);

public slots:
    void onSteamLightOn(bool);
    void onColorLightBtnClicked(int);
    void onLightIntensitySliderChanged(int);
    void onUserLoggedInLightBrightness(int);
    void onLightToggleStateChanged(bool);
    void onUserLoggedInLightToggle(bool);
    void onUserLoggedInLightMode(int);
    void onUserLoggedInRGB(int);
    void onUserLoggedInSteamRGB(int);
    void onUserLoggedInLightColor(int);
    void onUserLoggedInPreset1Color(int);
    void onUserLoggedInPreset2Color(int);
    void onUserLoggedInPreset3Color(int);
    void onUserLoggedInPreset1White(int);
    void onUserLoggedInPreset2White(int);
    void onUserLoggedInPreset3White(int);

    void onUserLoggedInSteamLightColor(int);
    void onUserLoggedInSteamLightBrightness(int);
    void onUserLoggedOut();
    void onColorWheelChange(QRgb);
    void onWhiteWheelChange(QRgb);


private:
    const int LIGHT_BRIGHTNESS_DEFAULT_VALUE = 100;
    int currentLightBrightnessValue;
    int currentSteamLightBrightnessValue;
    unsigned int currentLightMode;
    int currnetRGB;
    bool currentLightToggleStateOn;
    bool isSteamOn;
    bool isUserLoggedOut;
    HVSLRLightCanInterface *HVSLRlightCanInterface;
    Light lightButtonClicked;
    Light steamLightButtonClicked;
};

#endif // HVSLRLIGHTSCREENMODEL_H
