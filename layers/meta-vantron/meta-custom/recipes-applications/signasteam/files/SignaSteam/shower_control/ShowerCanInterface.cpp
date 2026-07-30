#include "ShowerCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include <QTimer>

#include <QDebug>
#include "development/DevFunctions.h"

ShowerCanInterface::ShowerCanInterface(QObject *parent) : QObject(parent)
{
    QTimer *controllerHeartBeat = new QTimer(this);
    connect(controllerHeartBeat, &QTimer::timeout, this, &ShowerCanInterface::sendControllerHeartBeat);
    controllerHeartBeat->start(5000);
}

void ShowerCanInterface::sendControllerHeartBeat()
{
    esv_send_heartbeat();
}

void ShowerCanInterface::turnOnShower()
{
    esv_turn_on();
}

void ShowerCanInterface::turnOffShower()
{
    esv_turn_off();
}

void ShowerCanInterface::changeWaterTemperature(double temp)
{
    int tempAsInt = static_cast<int>(temp);
    esv_set_temperature(static_cast<unsigned char>(tempAsInt));
}

void ShowerCanInterface::changeWaterFlow(int waterFlow)
{
    esv_set_flow(static_cast<unsigned char>(waterFlow));
}

void ShowerCanInterface::changeWaterOutput(int waterOutput)
{
    esv_set_output(static_cast<unsigned char>(waterOutput));
}
