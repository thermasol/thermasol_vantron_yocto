#ifndef LIGHTSCREENMODEL_H
#define LIGHTSCREENMODEL_H

#include "application/AppConstants.h"
#include "../util/lighthelper.h"

#include <QObject>

class LightCanInterface;

class LightScreenModel : public QObject
{
    Q_OBJECT

public:
    explicit LightScreenModel(QObject *parent = nullptr, LightCanInterface *lightCanInterface = nullptr);
    void setLightHelper(LightHelper& normal, LightHelper& steam);

signals:
    void lightChanged(LightMode);
    void lightBrightnessChanged(int);
    void lightToggleChanged(bool);
    void userLoggedOutLightBrightness(int);
    void userLoggedOutLightToggle(bool);
    void brightnessSliderLightToggle(bool);
    void userLoggedOutLightColor(int);
    void userLoggedOutSteamLightColor(int);
    void userLoggedOutSteamBrightness(int);
    void checkedState(bool);
    void updateLightHardware();

public slots:
    void onSteamLightOn(bool);
    void onColorLightBtnClicked(int);
    void onLightIntensitySliderChanged(int);
    void onUserLoggedInLightBrightness(int);
    void onLightToggleStateChanged(bool);
    void onUserLoggedInLightToggle(bool);
    void onUserLoggedInLightColor(int);
    void onUserLoggedInSteamLightColor(int);
    void onUserLoggedInSteamLightBrightness(int);
    void onUserLoggedOut();
    void updateSLSR();


private:
    const int LIGHT_BRIGHTNESS_DEFAULT_VALUE = 100;
    int currentLightBrightnessValue;
    int currentSteamLightBrightnessValue;
    bool currentLightToggleStateOn;
    bool isSteamOn;
    bool isUserLoggedOut;
    LightCanInterface *lightCanInterface;
   // Light lightButtonClicked;
    //LightMode steamLightButtonClicked;
    LightHelper *normal, *steam;
};

#endif // LIGHTSCREENMODEL_H
