#ifndef SETTINGSSTEAMGENERATORSCREEN_H
#define SETTINGSSTEAMGENERATORSCREEN_H

#include "application/AppConstants.h"

#include <QLabel>
#include <QToolButton>
#include <QWidget>

namespace Ui {
    class SettingsSteamGeneratorScreenLandscape;
    class SettingsSteamGeneratorScreenPortrait;
}

class EquipmentCanInterface;
class SettingsScreenModel;
class SteamCanInterface;

class SettingsSteamGeneratorScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsSteamGeneratorScreen(QWidget *parent = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr, SettingsScreenModel *settingsScreenModel = nullptr, SteamCanInterface *steamCanInterface = nullptr);
    ~SettingsSteamGeneratorScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);
    void pfStarted(bool);

public slots:
    void onFastStartUiChanged(FastStart);
    void onPfTimerChanged(int);
    void onPfFinished(QPair<PsbState, PsbErrorState>);
    void onUserLoggedOut();
    void onEquipmentDisconnected();

private slots:
    void onBackButtonClicked();
    void onPowerFlushBtnToggled(bool);

private:
    const int FONT_SIZE = 68;
    Ui::SettingsSteamGeneratorScreenLandscape *uiLandscape;
    Ui::SettingsSteamGeneratorScreenPortrait *uiPortrait;
    EquipmentCanInterface *equipmentCanInterface;
    SettingsScreenModel *settingsScreenModel;
    SteamCanInterface *steamCanInterface;
    QLabel *powerFlushMinsLeft;
    QLabel *powerFlushSecsLeft;
    QButtonGroup *fastStartBtnGroup;
    QToolButton *fastStartOffBtn;
    QToolButton *fastStartOnBtn;
    QToolButton *fastStartGreenBtn;
    QToolButton* backButton;
    QString orientationSelected;
    QToolButton* startStopPowerFlushBtn;
};

#endif // SETTINGSSTEAMGENERATORSCREEN_H
