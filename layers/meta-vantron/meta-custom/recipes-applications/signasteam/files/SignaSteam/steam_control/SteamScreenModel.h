#ifndef STEAMSCREENMODEL_H
#define STEAMSCREENMODEL_H

#include "application/AppConstants.h"

class SteamCanInterface;
class EquipmentCanInterface;

class SteamScreenModel : public QObject
{
    Q_OBJECT

public:
    explicit SteamScreenModel(QObject *parent = nullptr, SteamCanInterface *steamCanInterface = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    void onEquipmentDisconnected();

signals:
    void steamTempChanged(double);
    void steamTimeChanged(int);
    void userLoggedOutSteamTemp(double);
    void userLoggedOutSteamTime(int);
    void steamSessionTimedOut();

public slots:
    void onDecreaseTempBtnClicked();
    void onIncreaseTempBtnClicked();
    void onDecreaseTimeBtnClicked();
    void onIncreaseTimeBtnClicked();
    void onUserLoggedInSteamTemp(double);
    void onUserLoggedInSteamTime(int);
    void onSlideToStartSwiped();
    void onStopSessionClicked();
    void onTempScaleChanged(QString);
    void onUserLoggedOut();
private slots:
    void onTimeCountedDown();

private:
    const int TIME_SECONDS_MAX = 3600;
    const int TIME_SECONDS_MIN = 60;
    const int TIME_SECONDS_DEFAULT = 1200;
    const double TEMP_MAX = 120;
    const double TEMP_MIN = 80;
    const double STEAM_TEMP_DEFAULT = 100.0;
    double currentTemp = STEAM_TEMP_DEFAULT;
    int currentTime = TIME_SECONDS_DEFAULT;
    int previousStartTime = TIME_SECONDS_DEFAULT;
    bool tempScaleInF = true;
    SteamCanInterface *steamCanInterface;
    EquipmentCanInterface *equipmentCanInterface;
    QTimer *countdownTimer;
    void validateAndSendOffTemp();
    void sendTempToCan(double);
    void sendTimeToCan(int);
    void finishSteamSession();
    double validateTemp(double);
    int validateTime(int);
    double convertTempToC(double);
};

#endif // STEAMSCREENMODEL_H
