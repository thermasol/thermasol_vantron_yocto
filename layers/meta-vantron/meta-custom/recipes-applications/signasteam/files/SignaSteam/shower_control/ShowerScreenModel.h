#ifndef SHOWERSCREENMODEL_H
#define SHOWERSCREENMODEL_H

#include "application/AppConstants.h"

class ShowerCanInterface;

class ShowerScreenModel : public QObject
{
    Q_OBJECT

public:
    explicit ShowerScreenModel(QObject *parent = nullptr, ShowerCanInterface *showerCanInterface = nullptr);
    void onEquipmentDisconnected();

signals:
    void waterTempChanged(double);
    void waterFlowChanged(WaterFlow);
    void showerStateChanged(bool);
    void userLoggedOutWaterTemp(double);
    void userLoggedOutWaterFlow(WaterFlow);

public slots:
    void onDecreaseTempBtnClicked();
    void onIncreaseTempBtnClicked();
    void onDecreaseFlowBtnClicked();
    void onIncreaseFlowBtnClicked();
    void onUserLoggedInWaterTemp(double);
    void onUserLoggedInWaterFlow(WaterFlow);
    void onWaterOutputBtnClicked(int);
    void onTempScaleChanged(QString);
    void onUserLoggedOut();

private:
    const double TEMP_MAX = 120;
    const double TEMP_MIN = 59;
    const double TEMP_DEFAULT = 90;
    const double TEMP_LOW = 45;
    const WaterFlow WATER_FLOW_MIN = WaterFlow::LOW;
    const WaterFlow WATER_FLOW_MAX = WaterFlow::HIGH;
    const WaterFlow WATER_FLOW_DEFAULT = WaterFlow::MEDIUM;
    double currentTemp = TEMP_DEFAULT;
    int previousOutputId = -1;
    bool tempScaleInF = true;
    WaterFlow currentWaterFlow = WATER_FLOW_DEFAULT;
    ShowerCanInterface *showerCanInterface;
    void validateAndSendOffTemp();
    double validateTemp(double);
    WaterFlow validateFlow(WaterFlow);
    double convertTempToC(double);
};

#endif // SHOWERSCREENMODEL_H
