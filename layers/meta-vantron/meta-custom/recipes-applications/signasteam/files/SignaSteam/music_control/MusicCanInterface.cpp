#include "MusicCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include "settings_equipment/BluetoothCanInterface.h"

#include <QDebug>

MusicCanInterface::MusicCanInterface(QObject *parent, BluetoothCanInterface *bluetoothCanInterface) : QObject(parent)
{
    this->bluetoothCanInterface = bluetoothCanInterface;

    connect(this->bluetoothCanInterface, &BluetoothCanInterface::bluetoothStateChanged, this, &MusicCanInterface::onBluetoothStateChanged);



    QTimer *btAvailabilityTimer = new QTimer(this);
    connect(btAvailabilityTimer, &QTimer::timeout, this, &MusicCanInterface::checkBtEquipmentAvailability);
    btAvailabilityTimer->start(1000);
}

void MusicCanInterface::checkBtEquipmentAvailability()
{
    if(slsr_is_slsr_bluetooth_present() || hvlsr_is_active())
    {
        emit btAvailability(BluetoothEquipment::BT_EQUIPMENT_AVAILABLE);
    }
}

void MusicCanInterface::turnOnBt()
{   
    if(hvlsr_is_active())
    {
        hvlsr_bluetooth_power_on();
    }
    else
    {
        slsr_bluetooth_power_on();
    }
}

void MusicCanInterface::turnOffBt()
{
    if(hvlsr_is_active())
    {
        hvlsr_bluetooth_power_off();
    }
    else
    {
        slsr_bluetooth_power_off();

    }
}

void MusicCanInterface::btSkipBack()
{
    if(hvlsr_is_active())
    {
        hvlsr_bluetooth_skip_back();
    }
    else
    {
        slsr_bluetooth_skip_back();
    }
}

void MusicCanInterface::btPlay()
{
    if(hvlsr_is_active())
    {
        hvlsr_bluetooth_play();
    }
    else
    {
        slsr_bluetooth_play();
    }
}

void MusicCanInterface::btPause()
{
    if(hvlsr_is_active())
    {
        hvlsr_bluetooth_pause();
    }
    else
    {
        slsr_bluetooth_pause();
    }
}

void MusicCanInterface::btSkipForward()
{
    if(hvlsr_is_active())
    {
        hvlsr_bluetooth_skip_forward();
    }
    else
    {
        slsr_bluetooth_skip_forward();
    }
}


void MusicCanInterface::changeMusicInput(MusicInput input)
{
    if(hvlsr_is_active())
    {
       if(input == MusicInput::BLUETOOTH)
       {
           hvlsr_bluetooth_power_on();
       }
       else
       {
           hvlsr_bluetooth_power_off();
       }

       if(input == MusicInput::NOINPUT)
       {
           hvlsr_mute();
       }
       else
       {
           hvlsr_unmute();
       }
        hvlsr_set_source(static_cast<unsigned char>(input));
    }
    else
    {
        slsr_set_source(static_cast<unsigned char>(input));
    }
}

void MusicCanInterface::changeBass(int bass)
{
    if(hvlsr_is_active())
    {
        hvlsr_bass_volume(static_cast<unsigned char>(bass));
    }
    else
    {
        slsr_music_bass_tone(static_cast<unsigned char>(bass));
    }
}

void MusicCanInterface::changeMid(int mid)
{
    if(hvlsr_is_active())
    {
        hvlsr_mid_volume(static_cast<unsigned char>(mid));
    }
    else
    {
        slsr_music_mid_tone(static_cast<unsigned char>(mid));
    }
}

void MusicCanInterface::changeTreble(int treble)
{
    if(hvlsr_is_active())
    {
        hvlsr_treble_volume(static_cast<unsigned char>(treble));
    }
    else
    {
        slsr_music_treble_tone(static_cast<unsigned char>(treble));
    }
}

void MusicCanInterface::onBluetoothStateChanged(BluetoothState state)
{
    QString deviceName = "";
    QString inputStatus = "";
    switch (state) {
        case BluetoothState::BLUETOOTH_SEARCHING:
            inputStatus = "Bluetooth Searching";
            break;
        case BluetoothState::PAIRING:
            inputStatus = "Bluetooth Pairing";
            break;
        case BluetoothState::PAIRED:
            deviceName = bluetoothCanInterface->getPairedDeviceName();
            inputStatus = "Device Connected";
            break;
    }

    emit getInputState(inputStatus, deviceName);
}
