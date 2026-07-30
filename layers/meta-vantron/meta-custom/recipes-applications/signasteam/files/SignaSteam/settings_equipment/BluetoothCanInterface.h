#ifndef BLUETOOTHCANINTERFACE_H
#define BLUETOOTHCANINTERFACE_H

#include "application/AppConstants.h"

#include <QObject>

class BluetoothCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothCanInterface(QObject *parent = nullptr);
    QString getPairedDeviceName();

signals:
    void bluetoothStateChanged(BluetoothState);

public slots:

private slots:
    void onCheckBtStateTimeOut();

private:
    int testStatus = 0;
};

#endif // BLUETOOTHCANINTERFACE_H
