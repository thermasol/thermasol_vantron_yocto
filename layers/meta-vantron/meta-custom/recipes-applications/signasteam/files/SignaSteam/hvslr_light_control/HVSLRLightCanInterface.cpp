#include "HVSLRLightCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include <QDebug>
#include <QTimer>
#include <QThread>

HVSLRLightCanInterface::HVSLRLightCanInterface(QObject *parent) : QObject(parent)
{

    intensity = 0;
    rgb = 0;
    SteamRgb = 0;
    SteamIsOn = false;

     moodLightMode = false;
     whiteLightMode = false;


    //Set heart beat time and connect the time out event to send heart beat command
    QTimer *hvslrSendHeartBeat = new QTimer(this);
    connect(hvslrSendHeartBeat, &QTimer::timeout, this, &HVSLRLightCanInterface::sendHeartBeat);
    hvslrSendHeartBeat->start(1000);
}

void HVSLRLightCanInterface::turnLightOff()
{

    hvlsr_light_off();

}

void HVSLRLightCanInterface::turnLightOn()
{
    hvlsr_light_on();
    if(SteamIsOn)
    {
        if(whiteLightMode)
        {
            hvlsr_set_white_rbga(SteamRgb << 8  | (intensity & 0xFF));
        }
        else
        {
            hvlsr_set_rbga(SteamRgb << 8  | (intensity & 0xFF));
        }
    } else
    {
        if(whiteLightMode)
        {
            hvlsr_set_white_rbga(rgb << 8 | (intensity & 0xFF));
        }
        else
        {
            hvlsr_set_rbga(rgb << 8 | (intensity & 0xFF));
        }
    }

}

void HVSLRLightCanInterface::changeLight(Light light)
{

   // turnLightOn(); //required for old slsr

    whiteLightMode = false;
    moodLightMode = false;
    switch (light) {
        case Light::VIOLET:
            if(SteamIsOn)
            {
                SteamRgb = COLOR_VIOLET;
            }
            else {
                rgb = COLOR_VIOLET;
            }

            hvlsr_set_rbga(COLOR_VIOLET << 8| intensity);
            break;
        case Light::INDIGO:
            if(SteamIsOn)
            {
                SteamRgb = COLOR_INDIGO;
            }
            else {
                rgb = COLOR_INDIGO;
            }

            hvlsr_set_rbga(COLOR_INDIGO << 8| intensity);
            break;
        case Light::BLUE:
            if(SteamIsOn)
            {
                SteamRgb = COLOR_BLUE;
            }
            else {
                rgb = COLOR_BLUE;
            }
            hvlsr_set_rbga(COLOR_BLUE << 8| intensity);
            break;
        case Light::GREEN:
            if(SteamIsOn)
            {
                SteamRgb = COLOR_GREEN;
            }
            else {
                rgb = COLOR_GREEN;
            }
            hvlsr_set_rbga(COLOR_GREEN << 8| intensity);
            break;
        case Light::YELLOW:
            if(SteamIsOn)
            {
                SteamRgb = COLOR_YELLOW;
            }
            else {
                rgb = COLOR_YELLOW;
            }
            hvlsr_set_rbga(COLOR_YELLOW << 8| intensity);
            break;
        case Light::ORANGE:
            if(SteamIsOn)
            {
                SteamRgb = COLOR_ORANGE;
            }
            else {
                rgb = COLOR_ORANGE;
            }
            hvlsr_set_rbga(COLOR_ORANGE << 8| intensity);
            break;
        case Light::RED:
        if(SteamIsOn)
            {
                SteamRgb = COLOR_RED;
            }
            else {
                rgb = COLOR_RED;
            }
            hvlsr_set_rbga(COLOR_RED << 8| intensity);
            break;
        case Light::MOOD:
            moodLightMode = true;
            hvlsr_set_mood_light(0);
            break;
        case Light::WHITE:
            whiteLightMode = true;
            break;
        case Light::COLOR:
            whiteLightMode = false;
            break;
        case Light::CHROMA:

            break;
    }
}

void HVSLRLightCanInterface::changeLightIntensity(int lightIntensity)
{
    intensity = static_cast<unsigned char>(lightIntensity);
    int RGBValue;


    if(SteamIsOn)
    {
        RGBValue = SteamRgb;
    }
    else
    {
        RGBValue = rgb;
    }

    if(whiteLightMode)
    {
       hvlsr_set_white_rbga(RGBValue << 8 | intensity);
    }
    else if (moodLightMode)
    {
        hvlsr_set_rbga(intensity&0xff);
        hvlsr_set_mood_light(0);

    }
    else
    {
         hvlsr_set_rbga(RGBValue << 8 | intensity);

    }
}

void HVSLRLightCanInterface::setLightIntensity(int lightIntensity)
{
    intensity = static_cast<unsigned char>(lightIntensity);
}

void HVSLRLightCanInterface::changeSteamLightIntensity(int lightIntensity)
{
    intensity = static_cast<unsigned char>(lightIntensity);

    if(!whiteLightMode)
    {
        hvlsr_set_rbga(SteamRgb << 8 | intensity);
    }
    else
    {
        hvlsr_set_white_rbga(SteamRgb << 8 | intensity);
    }
}

void HVSLRLightCanInterface::sendHeartBeat()
{
    if(hvlsr_is_active())
    {
        hvlsr_send_heartbeat();
    }
}

void HVSLRLightCanInterface::setRGB(int r, int g, int b)
{
    unsigned int value = (r & 0xFF)<<16 | (g & 0xFF)<<8 | (b & 0xFF);

    rgb = value ;
}

void HVSLRLightCanInterface::setSteamRGB(int r, int g, int b)
{
    unsigned int value = (r & 0xFF)<<16 | (g & 0xFF)<<8 | (b & 0xFF);

    SteamRgb = value ;
}

void HVSLRLightCanInterface::setRGBTurnOn(int r, int g, int b)
{

    unsigned int value = (r & 0xFF)<<16 | (g & 0xFF)<<8 | (b & 0xFF);

    whiteLightMode = false;

    rgb = value ;

    hvlsr_set_rbga(rgb << 8 | (intensity & 0xFF));
}

void HVSLRLightCanInterface::setSteamRGBTurnOn(int r, int g, int b)
{

    unsigned int value = (r & 0xFF)<<16 | (g & 0xFF)<<8 | (b & 0xFF);

    whiteLightMode = false;

    SteamRgb = value ;

    hvlsr_set_rbga(SteamRgb << 8 | (intensity & 0xFF));

}

void HVSLRLightCanInterface::setWhiteRGBTurnOn(int r, int g, int b)
{

     unsigned int value = (r & 0xFF)<<16 | (g & 0xFF)<<8 | (b & 0xFF);

    whiteLightMode = true;

    rgb = value;

    hvlsr_set_white_rbga(rgb << 8 | (intensity & 0xFF));
}

void HVSLRLightCanInterface::setSteamWhiteRGBTurnOn(int r, int g, int b)
{

     unsigned int value = (r & 0xFF)<<16 | (g & 0xFF)<<8 | (b & 0xFF);

    whiteLightMode = true;

    SteamRgb = value;

    hvlsr_set_white_rbga(SteamRgb << 8 | (intensity & 0xFF));
}

unsigned int HVSLRLightCanInterface::getRGB()
{
    return rgb;
}

unsigned int HVSLRLightCanInterface::getSteamRGB()
{
    return SteamRgb;
}

void HVSLRLightCanInterface::setSteamIsOn(bool steamOn)
{
    SteamIsOn = steamOn;
}
