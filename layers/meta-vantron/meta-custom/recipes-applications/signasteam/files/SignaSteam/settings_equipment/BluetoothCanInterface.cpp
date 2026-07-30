#include "BluetoothCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include <QTimer>

#include <QDebug>
#include "development/DevFunctions.h"

BluetoothCanInterface::BluetoothCanInterface(QObject *parent) : QObject(parent)
{
    QTimer *checkBtStateTimer = new QTimer(this);
    connect(checkBtStateTimer, &QTimer::timeout, this, &BluetoothCanInterface::onCheckBtStateTimeOut);
    checkBtStateTimer->start(1000);

}

void BluetoothCanInterface::onCheckBtStateTimeOut()
{
    unsigned char blueToothState;

    if(hvlsr_is_active())
    {
        blueToothState = hvlsr_get_bluetooth_state();
    }
    else
    {
        blueToothState = slsr_get_bluetooth_state();
    }
    emit bluetoothStateChanged(static_cast<BluetoothState>(blueToothState));
}

QString BluetoothCanInterface::getPairedDeviceName()
{
    char* deviceName;
    if(hvlsr_is_active())
    {
        deviceName = (char*)hvlsr_bluetooth_get_devicename();
    }
    else
    {
        deviceName = (char*)slsr_bluetooth_get_devicename();
    }
    QString device = QString::fromUtf8(deviceName,sizeof(deviceName));
    return device;
}
