#ifndef SETTINGSSCREENMODEL_H
#define SETTINGSSCREENMODEL_H

#include "application/AppConstants.h"

#include <QSettings>
#include <QTime>

class EquipmentCanInterface;

class SettingsScreenModel : public QObject
{
    Q_OBJECT

public:
    explicit SettingsScreenModel(QObject *parent = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    void saveUsernameAndIcon();
    void setNewTime();

signals:
    void userLoggedInSteamTemp(double);
    void userLoggedInSteamTime(int);
    void userLoggedInWaterTemp(double);
    void userLoggedInTreble(int);
    void userLoggedInMid(int);
    void userLoggedInBass(int);
    void userLoggedInBtToggle(bool);
    void userLoggedInAuxToggle(bool);
    void userLoggedInVolume(int);
    void userLoggedInMuteToggle(bool);
    void userLoggedInVoiceVolume(int);
    void userLoggedInVoiceToggle(bool);
    void userLoggedInFastStart(int);
    void voiceVolumeSliderVoiceToggle(bool);

    void userLoggedInHVSLRLightBrightness(int);
    void userLoggedInHVSLRLightColor(int);

    void userLoggedInHVSLRLightToggle(bool);
    void userLoggedInHVSLRLightMode(int);
    void userLoggedInHVSLRRGB(int);
    void userLoggedInSteamHVSLRRGB(int);

    void userLoggedInHVSLRPreset1Color(int);
    void userLoggedInHVSLRPreset2Color(int);
    void userLoggedInHVSLRPreset3Color(int);
    void userLoggedInHVSLRPreset1White(int);
    void userLoggedInHVSLRPreset2White(int);
    void userLoggedInHVSLRPreset3White(int);

    void userLoggedInHVSLRSteamLightColor(int);
    void userLoggedInHVSLRSteamBrightness(int);

    void userLoggedInLightBrightness(int);
    void userLoggedInLightToggle(bool);
    void userLoggedInLightColor(int);
    void userLoggedInSteamLightColor(int);
    void userLoggedInSteamBrightness(int);
    void userLoggedInWaterFlow(WaterFlow);
    void voiceVolumeChanged(int);
    void voiceToggleChanged(bool);
    void fastStartChanged(FastStart);
    void tempScaleChanged(QString);
    void timeFormatChanged(QString);
    void timeChanged();
    void timeEditReset();
    void usernameChanged(QString);
    void userIconChanged(QString);
    void refreshUserScreen(User);
    void userLoggedOutVoiceVolume(int);
    void userLoggedOutVoiceToggle(bool);
    void userLoggedOutFastStart(int);

public slots:
    void onUserLoggedIn(User);
    void onUserLoggedOutSteamTemp(double);
    void onUserLoggedOutSteamTime(int);
    void onUserLoggedOutTreble(int);
    void onUserLoggedOutMid(int);
    void onUserLoggedOutBass(int);
    void onUserLoggedOutBtToggle(bool);
    void onUserLoggedOutAuxToggle(bool);
    void onUserLoggedOutVolume(int);
    void onUserLoggedOutMuteToggle(bool);
    void onUserLoggedOutVoiceVolume(int);
    void onUserLoggedOutVoiceToggle(bool);
    void onUserLoggedOutFastStart(int);
    void onUserLoggedOutHVSLRLightBrightness(int);
    void onUserLoggedOutHVSLRLightToggle(bool);
    void onUserLoggedOutHVSLRLightMode(int);
    void onUserLoggedOutHVSLRRGB(int);
    void onUserLoggedOutSteamHVSLRRGB(int);
    void onUserLoggedOutSteamHVSLRLightColor(int);
    void onUserLoggedOutSteamHVSLRBrightness(int);
    void onUserLoggedOutLightBrightness(int);
    void onUserLoggedOutLightToggle(bool);
    void onUserLoggedOutLightColor(int);
    void onUserLoggedOutSteamLightColor(int);
    void onUserLoggedOutSteamBrightness(int);
    void onUserLoggedOutWaterTemp(double);
    void onUserLoggedOutWaterFlow(WaterFlow);
    void onUserLoggedOutHVLSRLightColor(int);
    void onSavePreset1(int);
    void onSavePreset2(int);
    void onSavePreset3(int);
    void onSaveWhitePreset1(int);
    void onSaveWhitePreset2(int);
    void onSaveWhitePreset3(int);
    void onUserLoggedInVoiceVolume(int);
    void onUserLoggedInVoiceToggle(bool);
    void onUserLoggedInFastStart(int);
    void onTempScaleBtnClicked(int);
    void onTimeFormatBtnClicked(int);
    void onVoiceVolumeSliderChanged(int);
    void onVoiceToggleStateChanged(bool);
    void onFastStartChanged(int);
    void onTimeEditChanged(QTime);
    void onUsernameEntered(QString);
    void onUserIconDownBtnClicked();
    void onUserIconUpBtnClicked();
    void onOrientationChanged(Orientation);
    void onUserLoggedOut();

private:
    const int VOLUME_DEFAULT = 10;
    const int VOICE_VOLUME_DEFAULT = 10;
    const bool MUTE_TOGGLE_DEFAULT = true;
    const bool MUTE_VOICE_TOGGLE_DEFAULT = true;
    const int FAST_START_DEFAULT = 2;
    const int EQUALIZER_DEFAULT = 8;
    const bool BT_TOGGLE_DEFAULT = false;
    const bool AUX_TOGGLE_DEFAULT = false;
    const int LIGHT_BRIGHTNESS_DEFAULT = 100;
    const bool LIGHT_TOGGLE_DEFAULT = false;
    const int LIGHT_COLOR_DEFAULT = 8;
    const int HVSLR_LIGHT_MODE_DEFAULT = static_cast<int>(Light::COLOR);
    const int HVSLR_LIGHT_RGB_DEFAULT = 0xff5803;
    const int HVSLR_LIGHT_STEAM_RGB_DEFAULT = 0xff5803;
    const int LIGHT_PRESET_1_COLOR = 0x0016e6;
    const int LIGHT_PRESET_2_COLOR = 0xff5803;
    const int LIGHT_PRESET_3_COLOR = 0x23ff00;
    const int LIGHT_PRESET_1_WHITE = 0xadd7ee;
    const int LIGHT_PRESET_2_WHITE = 0xf6eac9;
    const int LIGHT_PRESET_3_WHITE = 0xfefefe;
    const bool HVLSR_LIGHT_TOGGLE_DEFAULT = true;
    const double STEAM_TEMP_DEFAULT = 100.0;
    const double WATER_TEMP_DEFAULT = 90;
    const int WATER_FLOW_DEFAULT = static_cast<int>(WaterFlow::MEDIUM);
    const int TIME_SECONDS_DEFAULT = 1200;
    const QList<QString> USER_ICON_PATHS = {
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Golf_Normal_126x126.png",
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Dragonfly_Normal_126x126 copy.png",
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Bird_Normal_126x126",
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Butterfly_Normal_126x126",
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_CowboyHat_Normal_126x126",
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Football_Normal_126x126",
        ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Paw_Normal_126x126"
    };
    const QString REBOOT_PACKAGE = "/sbin/reboot";
    int currentIconIndex;
    bool currentVoiceToggleState;
    int currentVoiceVolumeValue;
    FastStart currentFastStartValue;
    QTime newTime;
    QString loggedInUser;
    QString currentUsername;
    QSettings settings;
    EquipmentCanInterface *equipmentCanInterface;
    int validateUserIconIndex(int);
};

#endif // SETTINGSSCREENMODEL_H
