#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "AppConstants.h"

#include <QMap>

class EquipmentCanInterface;



class MainWindowModel : public QObject
{
    Q_OBJECT

public:
    explicit MainWindowModel(QObject *parent = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    bool isHVSLR();

signals:
    void usernameChanged(QString);
    void volumeValueChanged(int);
    void volumeSliderMuteToggleChanged(bool);
    void muteToggleChanged(bool);
    void clockTimeChanged(QString);
    void navBarChanged(QMap<NavBarButton, NavBarButtonState>);
    void userLoggedOutVolume(int);
    void userLoggedOutMuteToggle(bool);
    void pauseEzsSignal();

public slots:
    void onUserLoggedInVolume(int);
    void onVolumeBarSliderChanged(int);
    void onUserLoggedInMuteToggle(bool);
    void onMuteToggleStateChanged(bool);
    void onTimeChanged();
    void onTimeFormatChanged(QString);
    void onUserLoggedOut();
    void onEzsTurnOff();

private slots:
    void onDevicesStateChanged(QMap<CanDevice, CanDeviceState>);

private:
    int currentVolumeValue;
    bool currentMuteBtnStateValue;
    void turnOffEzs();
    EquipmentCanInterface *equipmentCanInterface;
    QMap<NavBarButton, NavBarButtonState> currentNavBarState;
    QString timeFormat;
    bool HVSLR;
};

#endif // MAINWINDOWMODEL_H
