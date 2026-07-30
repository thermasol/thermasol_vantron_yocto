#include "lighthelper.h"

LightHelper::LightHelper()
{
    lightOn = false;
    intensity = 0;
    lightMode = LightMode::WHITE;
    RGB = 0;
}

LightHelper::~LightHelper() {}

void LightHelper::turnlightOn()
{
     lightOn = true;
}

void  LightHelper::turnlightOff()
{
    lightOn = false;
}

bool  LightHelper::getLightOn()
{
    return lightOn;
}

void  LightHelper::setIntensity(int i)
{
    this->intensity = i;
}

int  LightHelper::getIntensity()
{
    return this->intensity;
}

void  LightHelper::setLightMode(LightMode mode)
{
    lightMode = mode;
}

LightMode  LightHelper::getLightMode()
{
    return lightMode;
}

void  LightHelper::setRGB(int rgb)
{
            this->RGB = rgb;
}

void  LightHelper::setRGB(int red, int green, int blue)
{
    this->RGB = ((red&0xFF)<<16) | ((green&0xFF)<<8) | (blue&0xFF);
}

int  LightHelper::getRGB()
{
    return this->RGB;
}

