#ifndef HVSLRLIGHTCANINTERFACE_H
#define HVSLRLIGHTCANINTERFACE_H

#include "application/AppConstants.h"

#include <QObject>

class HVSLRLightCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit HVSLRLightCanInterface(QObject *parent = nullptr);
    void turnLightOn();
    void turnLightOff();
    void changeLight(Light);
    void changeLightIntensity(int);
    void setLightIntensity(int);
    void changeSteamLightIntensity(int);
    void sendHeartBeat();
    void setRGB(int,int,int);
    void setSteamRGB(int,int,int);
    void setRGBTurnOn(int, int, int);
    void setSteamRGBTurnOn(int,int,int);
    void setSteamWhiteRGBTurnOn(int,int,int);
    void setWhiteRGBTurnOn(int, int, int);
    unsigned int getRGB();
    unsigned int getSteamRGB();
    void setSteamIsOn(bool);
 
private:
    unsigned char intensity;
    unsigned int rgb;
    unsigned int SteamRgb;

    const unsigned int COLOR_VIOLET = 0xFF00FF;
    const unsigned int COLOR_INDIGO = 0x800080;
    const unsigned int COLOR_BLUE   = 0x00007B;
    const unsigned int COLOR_GREEN  = 0x00FF00;
    const unsigned int COLOR_YELLOW = 0xFFFF00;
    const unsigned int COLOR_ORANGE = 0xFF5803;
    const unsigned int COLOR_RED    = 0xFF0000;

    bool whiteLightMode;
    bool moodLightMode;
    bool SteamIsOn;

};

#endif // HVSLRLIGHTCANINTERFACE_H
