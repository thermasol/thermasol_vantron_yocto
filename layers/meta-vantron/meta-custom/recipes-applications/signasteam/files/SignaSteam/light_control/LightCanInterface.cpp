#include "LightCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include <QDebug>

LightCanInterface::LightCanInterface(QObject *parent) : QObject(parent)
{
    lightMode = 1;
}

void LightCanInterface::turnLightOff()
{
    slsr_set_light_mode(0);
    slsr_turn_off_light();
}

void LightCanInterface::turnLightOn()
{
    slsr_turn_on_light();
}

void LightCanInterface::changeLight(LightMode light)
{



    switch (light) {
        case LightMode::VIOLET:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(6);
            break;
        case LightMode::INDIGO:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(5);
            break;
        case LightMode::BLUE:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(4);
            break;
        case LightMode::GREEN:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(3);
            break;
        case LightMode::YELLOW:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(2);
            break;
        case LightMode::ORANGE:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(1);
            break;
        case LightMode::RED:
            slsr_set_light_mode(4);
            lightMode = 4;
            slsr_chroma_state(0);
            break;
        case LightMode::MOOD:
            slsr_set_light_mode(2);
            lightMode = 2;
            break;
        case LightMode::WHITE:
            slsr_set_light_mode(1);
            lightMode = 1;
            break;
    }

}

void LightCanInterface::changeLightIntensity(int lightIntensity)
{
    slsr_set_light_intensity(static_cast<unsigned char>(lightIntensity));
}
