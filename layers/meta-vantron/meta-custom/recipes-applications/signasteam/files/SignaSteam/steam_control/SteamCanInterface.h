#ifndef STEAMCANINTERFACE_H
#define STEAMCANINTERFACE_H

#include <QObject>

class SteamCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit SteamCanInterface(QObject *parent = nullptr);
    void changeSteamTemperature(double);
    void changeSteamSessionTime(int);
    void startSteamSession();
    void stopSteamSession();
};

#endif // STEAMCANINTERFACE_H
