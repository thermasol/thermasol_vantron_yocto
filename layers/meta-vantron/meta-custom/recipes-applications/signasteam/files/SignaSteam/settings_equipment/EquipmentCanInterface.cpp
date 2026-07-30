#include "EquipmentCanInterface.h"

extern "C" {
    #include <thermacan.h>
}

#include "testing/TestBuild.h"

#include <QFile>
#include <QTimer>
#include <QtCore/qmath.h>

#include <QDebug>
#include "development/DevFunctions.h"

EquipmentCanInterface::EquipmentCanInterface(QObject *parent) : QObject(parent)
{
    canDevices[CanDevice::DIGITAL_SHOWER] = CanDeviceState::ABSENT;
    canDevices[CanDevice::STEAM_GENERATOR] = CanDeviceState::ABSENT;
    canDevices[CanDevice::SLSR] = CanDeviceState::ABSENT;
    canDevices[CanDevice::HVSLSR] = CanDeviceState::ABSENT;

    QTimer *controllerHeartBeat = new QTimer(this);
    connect(controllerHeartBeat, &QTimer::timeout, this, &EquipmentCanInterface::sendControllerHeartBeat);
    controllerHeartBeat->start(1000);

    QTimer *tempSensorTimer = new QTimer(this);
    connect(tempSensorTimer, &QTimer::timeout, this, &EquipmentCanInterface::onTempSensorTimerTimeOut);
    tempSensorTimer->start(1000);

    QTimer *checkDevicesTimer = new QTimer(this);
    connect(checkDevicesTimer, &QTimer::timeout, this, &EquipmentCanInterface::checkCanDeviceState);
    checkDevicesTimer->start(1000);

    powerFlushTimer = new QTimer(this);
    connect(powerFlushTimer, &QTimer::timeout, this, &EquipmentCanInterface::onPfTimer);

    psb_set_error(0);
    psb_set_current_room_temperature(80.0);
    thermacan_open();

    QTimer *checkPsbStateTimer = new QTimer(this);
    connect(checkPsbStateTimer, &QTimer::timeout, this, &EquipmentCanInterface::onCheckPsbStateTimeOut);
    checkPsbStateTimer->start(1000);

    QTimer *esvDebugTimer = new QTimer(this);
    connect(esvDebugTimer, &QTimer::timeout, this, &EquipmentCanInterface::onEsvDebugTimerTimeout);
    esvDebugTimer->start(10000);

    if(testBuild::isTestBuild) {
        QTimer::singleShot(500, this, &EquipmentCanInterface::checkCanDeviceState);
    }


}

void EquipmentCanInterface::setTemperatureFormat(int tempFormat){
    slsr_set_temperature_format(tempFormat);
}

void EquipmentCanInterface::checkCanDeviceState()
{

    if(esv_get_is_dongle_present() || inShowMode || testBuild::isTestBuild) {
        canDevices[CanDevice::DIGITAL_SHOWER] = CanDeviceState::PRESENT;
    } else {
        canDevices[CanDevice::DIGITAL_SHOWER] = CanDeviceState::ABSENT;
    }

    if((psb_get_is_psb_active() || inShowMode || testBuild::isTestBuild) && !psbInErrorState) {
        canDevices[CanDevice::STEAM_GENERATOR] = CanDeviceState::PRESENT;
    } else {
        canDevices[CanDevice::STEAM_GENERATOR] = CanDeviceState::ABSENT;
    }

    if(slsr_is_active())
    {
        canDevices[CanDevice::SLSR] = CanDeviceState::PRESENT;
        canDevices[CanDevice::HVSLSR] = CanDeviceState::ABSENT;
    }
    else if(hvlsr_is_active())
    {
        canDevices[CanDevice::HVSLSR] = CanDeviceState::PRESENT;
        canDevices[CanDevice::SLSR] = CanDeviceState::ABSENT;
    }
    else
    {
        //Neither SLSR or HVSRL are plugged in so default to HVSLR in show mode
        if(inShowMode)
        {
            canDevices[CanDevice::HVSLSR] = CanDeviceState::PRESENT;
            canDevices[CanDevice::SLSR] = CanDeviceState::ABSENT;
        }
        else
        {
           canDevices[CanDevice::HVSLSR] = CanDeviceState::ABSENT;
           canDevices[CanDevice::SLSR] = CanDeviceState::ABSENT;
        }

    }
    emit canDevicesStateChanged(canDevices);
}

void EquipmentCanInterface::sendControllerHeartBeat()
{
    psb_send_heartbeat();
}

FastStart EquipmentCanInterface::getFastStartModeFromCan()
{
    switch (psb_get_faststart_mode()) {
        case 0:
            return FastStart::OFF;
            break;
        case 2:
            return FastStart::ON;
            break;
        case 3:
            return FastStart::GREEN;
            break;
        default:
            return FastStart::GREEN;
            break;
    }
}

void EquipmentCanInterface::changeFastStartMode(FastStart fastStartMode)
{
    switch (fastStartMode) {
        case FastStart::OFF:
            psb_set_faststart_mode(0);
            break;
        case FastStart::ON:
            psb_set_faststart_mode(1);
            break;
        case FastStart::GREEN:
            psb_set_faststart_mode(2);
            break;
    }
}

void EquipmentCanInterface::turnOffEzs()
{
    //psb_user_logout();
}

void EquipmentCanInterface::startPowerFlush()
{
    psb_turn_on_power_flush();
    powerFlushTimer->start(1000);
}

void EquipmentCanInterface::stopPowerFlush()
{
    psb_turn_off_power_flush();
    powerFlushTimer->stop();
}

VoiceChip EquipmentCanInterface::getVoiceChipSLSR()
{
    unsigned int temp = slsr_get_version();
    if(temp <= 0x16)
    {
        return VoiceChip::VOICECHIP;
    }else
    {
        return VoiceChip::NOVOICECHIP;
    }
}

void EquipmentCanInterface::changeVolume(int volume)
{
    if(hvlsr_is_active())
    {

        hvlsr_master_volume(static_cast<unsigned char>(volume));
    }
    else
    {
        slsr_set_music_volume(static_cast<unsigned char>(volume));
    }
}

void EquipmentCanInterface::diagnosticsSendHello()
{
    psb_send_hello();
}

void EquipmentCanInterface::onShowModeBtnClicked(bool checked)
{
    if(checked){
        inShowMode = true;
    } else {
        inShowMode = false;
    }

    emit showModeState(inShowMode);
    checkCanDeviceState();
}

void EquipmentCanInterface::turnOnOffVoiceVolume(bool isOn)
{

    slsr_voice_on_off(static_cast<unsigned char>(false));

}

void EquipmentCanInterface::voiceMute(void)
{
    slsr_voice_mute();
   // version_temp = slsr_get_version();
}

void EquipmentCanInterface::voiceUnmute(void)
{
    slsr_voice_mute();
}

void EquipmentCanInterface::changeVoiceVolume(int value)
{

    slsr_set_voice_volume(static_cast<unsigned char>(0));
}

void EquipmentCanInterface::onPfTimer()
{
    emit pfTimerChanged(static_cast<int>(psb_get_power_flush_timer()));
}

void EquipmentCanInterface::onTempSensorTimerTimeOut()
{
    QFile tempSensorFile(TEMP_SENSOR_SYS_FILE);
    if (tempSensorFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&tempSensorFile);
        QString voltageRead = stream.readLine();

        double voltageIn = (voltageRead.toInt() * VOLTAGE_MAX) / ADC_MAX;
        double thermistorResistance = (PCB_RESISTANCE_BIAS * voltageIn) / (VOLTAGE_MAX - voltageIn);
        double tempInKelvin = B_FACTOR / qLn(thermistorResistance / R_ALPHA);
        double tempInCelcius = tempInKelvin - KELVIN_CONVERSION_FACTOR;
        double tempInFahrenheit = ((tempInCelcius * 9) / 5) + 32;

        psb_set_current_room_temperature(static_cast<float>(tempInFahrenheit));
        emit tempSensorRead(tempInFahrenheit);
    } else {
        psb_set_current_room_temperature(static_cast<float>(DEFAULT_TEMP_SENSOR_READING));
        emit tempSensorRead(DEFAULT_TEMP_SENSOR_READING);
    }

    tempSensorFile.close();
}

void EquipmentCanInterface::onCheckPsbStateTimeOut()
{
    psbCurrentState = psb_get_state();
    if(psbPreviousState != psbCurrentState){
        psbPreviousState = psbCurrentState;
        QPair<PsbState, PsbErrorState> psbState;
        switch (psbCurrentState) {
            case 2:
                psbState.first = PsbState::STEAM_ON;
                emit ezsSteamOnFlag();
                emit steamStarted(true);
                break;
            case 255:
                if(powerFlushInProgress) {
                    powerFlushInProgress = false;
                    psbState.first = PsbState::PF_FINISHED;
                    powerFlushTimer->stop();
                } else {
                    psbState.first = PsbState::STAND_BY;
                    emit steamStarted(false);
                }
                emit ezsSteamOffFlag();
                break;
            case 20:
                psbState.first = PsbState::PF_NEEDED;
                break;
            case 6:
                psbState.first = PsbState::PF_IN_PROGRESS;
                powerFlushInProgress = true;
                break;
            case 30:
                psbState.first = PsbState::PF_IN_PROGRESS;
                powerFlushInProgress = true;
                break;
            case 7:
                psbState.first = PsbState::PF_IN_PROGRESS;
                powerFlushInProgress = true;
                break;
            case 8:
                psbState.first = PsbState::PF_IN_PROGRESS;
                powerFlushInProgress = true;
                break;
            case 19:
                psbState.first = PsbState::ERROR;
                onPsbError(psbState);
                break;
        }
        emit powerFlushStarted(powerFlushInProgress);
        emit psbStateChanged(psbState);
    }
}

void EquipmentCanInterface::onPsbError(QPair<PsbState, PsbErrorState>& psbState)
{
    if(psb_get_errors() != 128) { // CAN error is considered no error
        switch (psb_get_errors()) {
            case 1:
                psbState.second = PsbErrorState::OVERTEMP;
                break;
            case 2:
                psbState.second = PsbErrorState::LOW_WATER;
                break;
            case 8:
                psbState.second = PsbErrorState::VOLTAGE_LOW;
                break;
        }
        psbInErrorState = true;
        checkCanDeviceState();
    } else {
        psbState.second = PsbErrorState::NO_ERROR;
    }
}

void EquipmentCanInterface::onEsvDebugTimerTimeout()
{
    emit dongleFirmwareVersion(esv_get_application_version());

    switch (esv_get_power_status()) {
        case 0:
            emit currentPowerStatus(ShowerValvePowerStatus::ON_LINE_POWER);
            break;
        case 1:
            emit currentPowerStatus(ShowerValvePowerStatus::ON_BATTERY_POWER);
            break;
        case 2:
            emit currentPowerStatus(ShowerValvePowerStatus::BATTERY_POWER_LOW);
            break;
        case 9:
            emit currentPowerStatus(ShowerValvePowerStatus::NO_POWER_INFO_FROM_VALVE);
            break;
    }

    emit valveConnectionStatus(esv_get_is_valve_present());
    emit waterFlowCount(static_cast<int>(esv_get_total_flow()));
    emit valveMeasuredTemp(esv_get_water_temperature());
}

void EquipmentCanInterface::mute()
{
     if(hvlsr_is_active())
         hvlsr_mute();
}

void EquipmentCanInterface::unmute()
{
     if(hvlsr_is_active())
        hvlsr_unmute();
}

