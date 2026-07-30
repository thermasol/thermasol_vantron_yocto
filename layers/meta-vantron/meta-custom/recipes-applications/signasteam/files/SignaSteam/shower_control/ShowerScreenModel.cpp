#include "ShowerScreenModel.h"

#include "ShowerCanInterface.h"

#include <QDebug>

ShowerScreenModel::ShowerScreenModel(QObject *parent, ShowerCanInterface *showerCanInterface)
    : QObject(parent)
{
    this->showerCanInterface = showerCanInterface;
}

void ShowerScreenModel::onEquipmentDisconnected()
{
    if(previousOutputId != -1) {
        onWaterOutputBtnClicked(previousOutputId);
    }
}

WaterFlow operator++(WaterFlow& waterFlow, int)
{
    waterFlow = static_cast<WaterFlow>(static_cast<int>(waterFlow) + 1);

    return waterFlow;
}

WaterFlow operator--(WaterFlow& waterFlow, int)
{
    waterFlow = static_cast<WaterFlow>(static_cast<int>(waterFlow) - 1);

    return waterFlow;
}

void ShowerScreenModel::onDecreaseTempBtnClicked()
{
    if(tempScaleInF) {
        currentTemp--;
    } else {
        currentTemp -= 1.8;
    }
    validateAndSendOffTemp();
}

void ShowerScreenModel::onIncreaseTempBtnClicked()
{
    if(tempScaleInF) {
        currentTemp++;
    } else {
        currentTemp += 1.8;
    }
    validateAndSendOffTemp();
}

void ShowerScreenModel::onDecreaseFlowBtnClicked()
{
    currentWaterFlow--;
    currentWaterFlow = validateFlow(currentWaterFlow);
    showerCanInterface->changeWaterFlow(static_cast<int>(currentWaterFlow));
    emit waterFlowChanged(currentWaterFlow);
}

void ShowerScreenModel::onIncreaseFlowBtnClicked()
{
    currentWaterFlow++;
    currentWaterFlow = validateFlow(currentWaterFlow);
    showerCanInterface->changeWaterFlow(static_cast<int>(currentWaterFlow));
    emit waterFlowChanged(currentWaterFlow);
}

void ShowerScreenModel::onUserLoggedInWaterTemp(double tempFromDb)
{
    currentTemp = tempFromDb;
    validateAndSendOffTemp();
}

void ShowerScreenModel::onUserLoggedInWaterFlow(WaterFlow waterFlowFromDb)
{
    currentWaterFlow = waterFlowFromDb;
    showerCanInterface->changeWaterFlow(static_cast<int>(currentWaterFlow));
    emit waterFlowChanged(currentWaterFlow);
}

void ShowerScreenModel::onWaterOutputBtnClicked(int waterOutputClickedAsInt)
{
    bool isShowerOn;
    showerCanInterface->changeWaterOutput(waterOutputClickedAsInt);

    if(previousOutputId == waterOutputClickedAsInt) {
        showerCanInterface->turnOffShower();
        previousOutputId = -1;
        isShowerOn = false;
        emit showerStateChanged(isShowerOn);
    } else {
        if(previousOutputId == -1) {
            showerCanInterface->turnOnShower();
            isShowerOn = true;
            emit showerStateChanged(isShowerOn);
        }
        previousOutputId = waterOutputClickedAsInt;
    }
}

void ShowerScreenModel::onTempScaleChanged(QString tempScale)
{
    if(tempScale == appPersistence::TEMP_SCALE_F_VALUE) {
        tempScaleInF = true;
    } else {
        tempScaleInF = false;
    }

    validateAndSendOffTemp();
}

void ShowerScreenModel::onUserLoggedOut()
{
    onEquipmentDisconnected();

    emit userLoggedOutWaterTemp(currentTemp);
    emit userLoggedOutWaterFlow(currentWaterFlow);
}

void ShowerScreenModel::validateAndSendOffTemp()
{
    currentTemp = validateTemp(currentTemp);

    if(static_cast<int>(currentTemp) == static_cast<int>(TEMP_MIN)) {
        showerCanInterface->changeWaterTemperature(TEMP_LOW);
        emit waterTempChanged(TEMP_LOW);
    } else {
        showerCanInterface->changeWaterTemperature(currentTemp);
        if(tempScaleInF) {
            emit waterTempChanged(currentTemp);
        } else {
            emit waterTempChanged(convertTempToC(currentTemp));
        }
    }
}

double ShowerScreenModel::validateTemp(double temp)
{
    if(temp > TEMP_MAX) {
        temp = TEMP_MAX;
    } else if(temp < TEMP_MIN) {
        temp = TEMP_MIN;
    }

    return temp;
}

WaterFlow ShowerScreenModel::validateFlow(WaterFlow waterFlow)
{
    if(static_cast<int>(waterFlow) < static_cast<int>(WATER_FLOW_MIN)) {
        waterFlow = WATER_FLOW_MIN;
    } else if(static_cast<int>(waterFlow) > static_cast<int>(WATER_FLOW_MAX)) {
        waterFlow = WATER_FLOW_MAX;
    }

    return waterFlow;
}

double ShowerScreenModel::convertTempToC(double temp)
{
    return (temp - 32.0) / 1.8;
}
