#ifndef MUSICSCREENMODEL_H
#define MUSICSCREENMODEL_H

#include "application/AppConstants.h"

#include <QObject>
#include <QSettings>
#include <QString>
#include <QTimer>

class MusicCanInterface;

class MusicScreenModel : public QObject
{
    Q_OBJECT

public:
    explicit MusicScreenModel(QObject *parent = nullptr, MusicCanInterface *musicCanInterface = nullptr);

signals:
    void trebleValueChanged(int);
    void midValueChanged(int);
    void bassValueChanged(int);
    void musicInputStateChange(QString, QString);
    void musicStateChanged(bool);
    void userLoggedOutTreble(int);
    void userLoggedOutMid(int);
    void userLoggedOutBass(int);
    void userLoggedOutBtToggle(bool);
    void userLoggedOutAuxToggle(bool);
    void auxChanged(bool);
    void btChanged(bool);
    void btEquipmentDetected(bool);

public slots:
    void onBluetoothAvailabilityReceived(BluetoothEquipment);
    void onMusicInputRecieved(QString, QString);
    void onTrackSkipBackBtnClicked();
    void onPauseBtnPressed();
    void onPlayBtnPressed();
    void onTrackSkipForwardBtnClicked();
    void onTrebleControlSliderChanged(int);
    void onMidControlSliderChanged(int);
    void onBassControlSliderChanged(int);
    void onBtToggleStateChanged(bool);
    void onAuxToggleStateChanged(bool);
    void onUserLoggedInTreble(int);
    void onUserLoggedInMid(int);
    void onUserLoggedInBass(int);
    void onUserLoggedInBtToggle(bool);
    void onUserLoggedInAuxToggle(bool);
    void onUserLoggedOut();
    void onShowModeChecked(bool);

private:
    const int EQUALIZER_DEFAULT_VALUE = 8;
    int currentTrebleValue;
    int currentMidValue;
    int currentBassValue;
    bool inShowMode;
    bool currentBtInputState;
    bool currentAuxInputState;
    bool isBluetoothAvailable;
    void handleMusicInputState();
    QTimer *checkMusicInputStateTimer;
    QString loggedInUser;
    QSettings settings;
    MusicCanInterface *musicCanInterface;
};

#endif // MUSICSCREENMODEL_H
