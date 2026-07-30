#include "SteamCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include <QDebug>
#include "development/DevFunctions.h"
#include <math.h>

SteamCanInterface::SteamCanInterface(QObject *parent) : QObject(parent)
{

}

void SteamCanInterface::changeSteamTemperature(double temp)
{
    double steamTemp;
    steamTemp = static_cast<double>(temp);
    psb_set_temperature_setpoint(static_cast<unsigned char>(steamTemp));
}

void SteamCanInterface::changeSteamSessionTime(int timeInSecs)
{
    int time = timeInSecs / 60;
    psb_set_timer_setpoint(static_cast<unsigned char>(time));
}

void SteamCanInterface::startSteamSession()
{
    psb_turn_on_steam(0);
}

void SteamCanInterface::stopSteamSession()
{
    psb_turn_off_steam();
}
