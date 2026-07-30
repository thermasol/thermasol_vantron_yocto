#ifndef EQUIPMENTCANINTERFACE_H
#define EQUIPMENTCANINTERFACE_H

#include "application/AppConstants.h"

#include <QMap>
#include <QTimer>

class EquipmentCanInterface : public QObject
{
    Q_OBJECT

public:
    explicit EquipmentCanInterface(QObject *parent = nullptr);
    void setTemperatureFormat(int);
    void checkCanDeviceState();
    void sendControllerHeartBeat();
    FastStart getFastStartModeFromCan();
    VoiceChip getVoiceChipSLSR();
    void changeFastStartMode(FastStart);
    void turnOffEzs();
    void startPowerFlush();
    void stopPowerFlush();
    void changeVolume(int);
    void diagnosticsSendHello();
    void mute();
    void unmute();


signals:
    void canDevicesStateChanged(QMap<CanDevice, CanDeviceState>);
    void powerFlushStarted(bool);
    void pfTimerChanged(int);
    void tempSensorRead(double);
    void psbStateChanged(QPair<PsbState, PsbErrorState>);
    void dongleFirmwareVersion(int);
    void valveConnectionStatus(bool);
    void currentPowerStatus(ShowerValvePowerStatus);
    void waterFlowCount(int);
    void valveMeasuredTemp(int);
    void ezsSteamOnFlag();
    void ezsSteamOffFlag();
    void steamStarted(bool);
    void showModeState(bool);

public slots:
    void onShowModeBtnClicked(bool);
    void turnOnOffVoiceVolume(bool);
    void voiceMute(void);
    void voiceUnmute(void);
    void changeVoiceVolume(int);

private slots:
    void onPfTimer();
    void onTempSensorTimerTimeOut();
    void onCheckPsbStateTimeOut();
    void onPsbError(QPair<PsbState, PsbErrorState>& psbState);
    void onEsvDebugTimerTimeout();

private:
    const double VOLTAGE_MAX = 1.8;
    const double ADC_MAX = 4096;
    const double PCB_RESISTANCE_BIAS = 1000;
    const double B_FACTOR = 3650;
    const double R_ALPHA = 0.004793;
    const double KELVIN_CONVERSION_FACTOR = 273.15;
    const double DEFAULT_TEMP_SENSOR_READING = 80.0;
    const QString TEMP_SENSOR_SYS_FILE = "/sys/bus/iio/devices/iio:device0/in_voltage0_raw";
    bool psbInErrorState = false;
    bool inShowMode = false;
    bool powerFlushInProgress = false;
    int psbCurrentState;
    int psbPreviousState = 0;
    QTimer *powerFlushTimer;
    QTimer *ezsUserTimer;
    QMap<CanDevice, CanDeviceState> canDevices;
    QMap<PsbState, PsbErrorState> psbState;
};

#endif // EQUIPMENTCANINTERFACE_H
