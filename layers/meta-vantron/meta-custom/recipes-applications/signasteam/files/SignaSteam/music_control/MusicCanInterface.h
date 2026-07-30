#ifndef MUSICCANINTERFACE_H
#define MUSICCANINTERFACE_H

#include "application/AppConstants.h"

#include <QObject>
#include <QTimer>
#include <QString>

class BluetoothCanInterface;

class MusicCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit MusicCanInterface(QObject *parent = nullptr, BluetoothCanInterface *bluetoothCanInterface = nullptr);
    void turnOnBt();
    void turnOffBt();
    void changeMusicInput(MusicInput);
    void btSkipBack();
    void btPlay();
    void btPause();
    void btSkipForward();
    void changeBass(int);
    void changeMid(int);
    void changeTreble(int);


signals:
    void getInputState(QString, QString);
    void btAvailability(BluetoothEquipment);

private slots:
    void onBluetoothStateChanged(BluetoothState);
    void checkBtEquipmentAvailability();

private:
    BluetoothCanInterface *bluetoothCanInterface;
    QTimer *btDisplayDataTimer;
    void getDeviceName();
};

#endif // MUSICCANINTERFACE_H
