#ifndef LIGHTCANINTERFACE_H
#define LIGHTCANINTERFACE_H

#include "application/AppConstants.h"
#include "../util/lighthelper.h"

#include <QObject>

class LightCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit LightCanInterface(QObject *parent = nullptr);
    void turnLightOn();
    void turnLightOff();
    void changeLight(LightMode);
    void changeLightIntensity(int);
    int lightMode;
};

#endif // LIGHTCANINTERFACE_H
