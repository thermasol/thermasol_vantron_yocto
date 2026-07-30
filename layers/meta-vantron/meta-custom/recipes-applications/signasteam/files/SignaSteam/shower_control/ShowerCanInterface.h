#ifndef SHOWERCANINTERFACE_H
#define SHOWERCANINTERFACE_H

#include "application/AppConstants.h"

#include <QObject>

class ShowerCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit ShowerCanInterface(QObject *parent = nullptr);
    void sendControllerHeartBeat();
    void turnOnShower();
    void turnOffShower();
    void changeWaterTemperature(double);
    void changeWaterFlow(int);
    void changeWaterOutput(int);
};

#endif // SHOWERCANINTERFACE_H
