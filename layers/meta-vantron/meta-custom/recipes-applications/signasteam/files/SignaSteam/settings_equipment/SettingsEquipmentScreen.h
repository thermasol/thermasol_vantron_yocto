#ifndef SETTINGSEQUIPMENTSCREEN_H
#define SETTINGSEQUIPMENTSCREEN_H

#include "application/AppConstants.h"

#include <QMap>
#include <QToolButton>
#include <QWidget>

namespace Ui {
    class SettingsEquipmentScreenLandscape;
    class SettingsEquipmentScreenPortrait;
}

class EquipmentCanInterface;

class SettingsEquipmentScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsEquipmentScreen(QWidget *parent = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    ~SettingsEquipmentScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);
    void equipmentDisconnected();

public slots:
    void onCanDevicesStateChanged(QMap<CanDevice, CanDeviceState>);

private slots:
    void onBackButtonClicked();
    void onSettingsEquipmentBtnClicked(int);

private:
    Ui::SettingsEquipmentScreenLandscape *uiLandscape;
    Ui::SettingsEquipmentScreenPortrait *uiPortrait;
    QToolButton *digitalShowerBtnClickArea;
    QToolButton *digitalShowerBtnImg;
    QToolButton *digitalShowerBtnLabel;
    QToolButton *steamGeneratorBtnClickArea;
    QToolButton *steamGeneratorBtnImg;
    QToolButton *steamGeneratorBtnLabel;
    QToolButton *slsrBtnClickArea;
    QToolButton *slsrBtnImg;
    QToolButton *slsrBtnLabel;
    QToolButton *hvslrBtnLabel;
    QString orientationSelected;
};

#endif // SETTINGSEQUIPMENTSCREEN_H
