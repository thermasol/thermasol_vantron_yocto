#include "SettingsScreenModel.h"

#include "../settings_equipment/EquipmentCanInterface.h"

#include "testing/TestBuild.h"

#include <QApplication>
#include <QProcess>
#include <QDebug>

SettingsScreenModel::SettingsScreenModel(QObject *parent, EquipmentCanInterface *equipmentCanInterface)
    : QObject(parent)
{
    this->equipmentCanInterface = equipmentCanInterface;
}

void SettingsScreenModel::saveUsernameAndIcon()
{
    settings.setValue(appPersistence::USERNAME_ICON_KEY + loggedInUser, USER_ICON_PATHS[currentIconIndex]);
    settings.setValue(appPersistence::USERNAME_KEY + loggedInUser, currentUsername);

    if(loggedInUser == appPersistence::USER_ONE) {
        emit refreshUserScreen(User::USER_ONE);
        emit usernameChanged(settings.value(appPersistence::USERNAME_KEY + loggedInUser, appPersistence::USERNAME_ONE_DEFAULT_VALUE).toString());
    } else if(loggedInUser == appPersistence::USER_TWO) {
        emit refreshUserScreen(User::USER_TWO);
        emit usernameChanged(settings.value(appPersistence::USERNAME_KEY + loggedInUser, appPersistence::USERNAME_TWO_DEFAULT_VALUE).toString());
    }
}

void SettingsScreenModel::setNewTime()
{
    QProcess::execute("date +%T -s " + newTime.toString());
    emit timeChanged();
}

void SettingsScreenModel::onUserLoggedIn(User user)
{
    equipmentCanInterface->turnOnOffVoiceVolume(0);
    equipmentCanInterface->changeVolume(0);
    switch (user) {
        case User::USER_ONE:
            loggedInUser = appPersistence::USER_ONE;
            currentUsername = settings.value(appPersistence::USERNAME_KEY + loggedInUser, appPersistence::USERNAME_ONE_DEFAULT_VALUE).toString();
            currentIconIndex = USER_ICON_PATHS.indexOf(settings.value(appPersistence::USERNAME_ICON_KEY + loggedInUser, appPersistence::USERNAME_ONE_ICON_DEFAULT_VALUE).toString());
            break;
        case User::USER_TWO:
            loggedInUser = appPersistence::USER_TWO;
            currentUsername = settings.value(appPersistence::USERNAME_KEY + loggedInUser, appPersistence::USERNAME_TWO_DEFAULT_VALUE).toString();
            currentIconIndex = USER_ICON_PATHS.indexOf(settings.value(appPersistence::USERNAME_ICON_KEY + loggedInUser, appPersistence::USERNAME_TWO_ICON_DEFAULT_VALUE).toString());
            break;

    }

    emit userLoggedInHVSLRLightBrightness(settings.value(appPersistence::HVSL_BRIGHTNESS_KEY + loggedInUser, LIGHT_BRIGHTNESS_DEFAULT).toInt());
    emit userLoggedInLightBrightness(settings.value(appPersistence::LIGHT_BRIGHTNESS_KEY + loggedInUser, LIGHT_BRIGHTNESS_DEFAULT).toInt());
    emit userLoggedInLightToggle(settings.value(appPersistence::LIGHT_TOGGLE_KEY + loggedInUser, LIGHT_TOGGLE_DEFAULT).toBool());
    emit userLoggedInLightColor(settings.value(appPersistence::LIGHT_COLOR_KEY + loggedInUser, LIGHT_COLOR_DEFAULT).toInt());

    emit userLoggedInHVSLRLightColor(settings.value(appPersistence::HVSLR_LIGHT_COLOR_KEY + loggedInUser, LIGHT_COLOR_DEFAULT).toInt());
    emit userLoggedInHVSLRLightToggle(settings.value(appPersistence::HVSLR_LIGHT_TOGGLE_KEY + loggedInUser, HVLSR_LIGHT_TOGGLE_DEFAULT).toBool());
    emit userLoggedInHVSLRLightMode(settings.value(appPersistence::HVSLR_LIGHT_MODE + loggedInUser, HVSLR_LIGHT_MODE_DEFAULT).toInt());
    emit userLoggedInHVSLRRGB(settings.value(appPersistence::HVSLR_LIGHT_RGB + loggedInUser, HVSLR_LIGHT_RGB_DEFAULT).toInt());

    emit userLoggedInSteamHVSLRRGB(settings.value(appPersistence::HVSLR_LIGHT_STEAM_RGB + loggedInUser, HVSLR_LIGHT_STEAM_RGB_DEFAULT).toInt());

    emit userLoggedInHVSLRPreset1Color(settings.value(appPersistence::HVSLR_PRESET_1_COLOR + loggedInUser, LIGHT_PRESET_1_COLOR).toInt());
    emit userLoggedInHVSLRPreset2Color(settings.value(appPersistence::HVSLR_PRESET_2_COLOR + loggedInUser, LIGHT_PRESET_2_COLOR).toInt());
    emit userLoggedInHVSLRPreset3Color(settings.value(appPersistence::HVSLR_PRESET_3_COLOR + loggedInUser, LIGHT_PRESET_3_COLOR).toInt());
    emit userLoggedInHVSLRPreset1White(settings.value(appPersistence::HVSLR_PRESET_1_WHITE + loggedInUser, LIGHT_PRESET_1_WHITE).toInt());
    emit userLoggedInHVSLRPreset2White(settings.value(appPersistence::HVSLR_PRESET_2_WHITE + loggedInUser, LIGHT_PRESET_2_WHITE).toInt());
    emit userLoggedInHVSLRPreset3White(settings.value(appPersistence::HVSLR_PRESET_3_WHITE + loggedInUser, LIGHT_PRESET_3_WHITE).toInt());

    emit userLoggedInHVSLRSteamLightColor(settings.value(appPersistence::HVSLR_STEAM_LIGHT_COLOR_KEY + loggedInUser,LIGHT_COLOR_DEFAULT).toInt());
    emit userLoggedInHVSLRSteamBrightness(settings.value(appPersistence::HVSL_STEAM_BRIGHTNESS_KEY + loggedInUser,LIGHT_BRIGHTNESS_DEFAULT).toInt());

    emit userLoggedInSteamLightColor(settings.value(appPersistence::STEAM_LIGHT_COLOR_KEY + loggedInUser, LIGHT_COLOR_DEFAULT).toInt());
    emit userLoggedInSteamBrightness(settings.value(appPersistence::STEAM_BRIGHTNESS_KEY + loggedInUser, LIGHT_BRIGHTNESS_DEFAULT).toInt());
    emit userLoggedInSteamTemp(settings.value(appPersistence::STEMP_TEMP_KEY + loggedInUser, STEAM_TEMP_DEFAULT).toDouble());
    emit userLoggedInSteamTime(settings.value(appPersistence::STEAM_TIME_KEY + loggedInUser, TIME_SECONDS_DEFAULT).toInt());   
    emit userLoggedInWaterTemp(settings.value(appPersistence::WATER_TEMP_KEY + loggedInUser, WATER_TEMP_DEFAULT).toDouble());
    emit userLoggedInTreble(settings.value(appPersistence::MUSIC_TREBLE_KEY + loggedInUser, EQUALIZER_DEFAULT).toInt());
    emit userLoggedInMid(settings.value(appPersistence::MUSIC_MID_KEY + loggedInUser, EQUALIZER_DEFAULT).toInt());
    emit userLoggedInBass(settings.value(appPersistence::MUSIC_BASS_KEY + loggedInUser, EQUALIZER_DEFAULT).toInt());
    emit userLoggedInBtToggle(settings.value(appPersistence::BT_INPUT_KEY + loggedInUser, BT_TOGGLE_DEFAULT).toBool());
    emit userLoggedInAuxToggle(settings.value(appPersistence::AUX_INPUT_KEY + loggedInUser, AUX_TOGGLE_DEFAULT).toBool());
    emit userLoggedInFastStart(settings.value(appPersistence::FAST_START_KEY, FAST_START_DEFAULT).toInt());

    switch (settings.value(appPersistence::WATER_FLOW_KEY + loggedInUser, WATER_FLOW_DEFAULT).toInt()) {
        case 0:
            emit userLoggedInWaterFlow(WaterFlow::LOW);
            break;
        case 1:
            emit userLoggedInWaterFlow(WaterFlow::MEDIUM);
            break;
        case 2:
            emit userLoggedInWaterFlow(WaterFlow::HIGH);
            break;
    }

    emit tempScaleChanged(settings.value(appPersistence::TEMP_SCALE_KEY + loggedInUser, appPersistence::TEMP_SCALE_DEFAULT_VALUE).toString());
    emit timeFormatChanged(settings.value(appPersistence::TIME_FORMAT_KEY + loggedInUser, appPersistence::TIME_FORMAT_DEFAULT_VALUE).toString());
    emit usernameChanged(currentUsername);
    emit userIconChanged(USER_ICON_PATHS[currentIconIndex]);

    emit userLoggedInVolume(settings.value(appPersistence::VOLUME_KEY + loggedInUser, VOLUME_DEFAULT).toInt());
    emit userLoggedInMuteToggle(settings.value(appPersistence::MUTE_SOUND_BTN_KEY + loggedInUser, MUTE_TOGGLE_DEFAULT).toBool());
    emit userLoggedInVoiceVolume(settings.value(appPersistence::VOICE_VOLUME_KEY + loggedInUser, VOICE_VOLUME_DEFAULT).toInt());
    emit userLoggedInVoiceToggle(settings.value(appPersistence::MUTE_VOICE_KEY + loggedInUser, MUTE_VOICE_TOGGLE_DEFAULT).toBool());

    newTime = QTime(0, 0);
    emit timeEditReset();

}

void SettingsScreenModel::onUserLoggedOutSteamTemp(double tempToStore)
{
    settings.setValue(appPersistence::STEMP_TEMP_KEY + loggedInUser, tempToStore);
}

void SettingsScreenModel::onUserLoggedOutSteamTime(int timeToStore)
{
    settings.setValue(appPersistence::STEAM_TIME_KEY + loggedInUser, timeToStore);
}

void SettingsScreenModel::onUserLoggedOutWaterTemp(double tempToStore)
{
    settings.setValue(appPersistence::WATER_TEMP_KEY + loggedInUser, tempToStore);
}

void SettingsScreenModel::onUserLoggedOutWaterFlow(WaterFlow flowToStore)
{
    settings.setValue(appPersistence::WATER_FLOW_KEY + loggedInUser, static_cast<int>(flowToStore));
}

void SettingsScreenModel::onUserLoggedOutTreble(int trebleToStore)
{
    settings.setValue(appPersistence::MUSIC_TREBLE_KEY + loggedInUser, trebleToStore);
}

void SettingsScreenModel::onUserLoggedOutMid(int midToStore)
{
    settings.setValue(appPersistence::MUSIC_MID_KEY + loggedInUser, midToStore);
}

void SettingsScreenModel::onUserLoggedOutBass(int bassToStore)
{
    settings.setValue(appPersistence::MUSIC_BASS_KEY + loggedInUser, bassToStore);
}

void SettingsScreenModel::onUserLoggedOutBtToggle(bool btButtonStateToStore)
{
    settings.setValue(appPersistence::BT_INPUT_KEY + loggedInUser, btButtonStateToStore);
}

void SettingsScreenModel::onUserLoggedOutAuxToggle(bool auxButtonStateToStore)
{
    settings.setValue(appPersistence::AUX_INPUT_KEY + loggedInUser, auxButtonStateToStore);
}

void SettingsScreenModel::onUserLoggedOutVolume(int volumeToStore)
{
    settings.setValue(appPersistence::VOLUME_KEY + loggedInUser, volumeToStore);
}

void SettingsScreenModel::onUserLoggedOutMuteToggle(bool muteBtnStateToStore)
{
    settings.setValue(appPersistence::MUTE_SOUND_BTN_KEY + loggedInUser, muteBtnStateToStore);
}

void SettingsScreenModel::onUserLoggedOutVoiceVolume(int voiceVolumeToStore)
{
    settings.setValue(appPersistence::VOICE_VOLUME_KEY + loggedInUser, voiceVolumeToStore);
}

void SettingsScreenModel::onUserLoggedOutVoiceToggle(bool voiceToggleState)
{
    settings.setValue(appPersistence::MUTE_VOICE_KEY + loggedInUser, voiceToggleState);
}

void SettingsScreenModel::onUserLoggedOutFastStart(int fastStartValueToStore)
{
    settings.setValue(appPersistence::FAST_START_KEY, fastStartValueToStore);
}

void SettingsScreenModel::onUserLoggedOutHVSLRLightBrightness(int lightBrightToStore)
{
    settings.setValue(appPersistence::HVSL_BRIGHTNESS_KEY + loggedInUser, lightBrightToStore);
}

void SettingsScreenModel::onUserLoggedOutHVSLRLightToggle(bool lightToggleStateToStore)
{
    settings.setValue(appPersistence::HVSLR_LIGHT_TOGGLE_KEY + loggedInUser, lightToggleStateToStore);
}

void SettingsScreenModel::onUserLoggedOutHVSLRLightMode(int lightModeToStore)
{
    settings.setValue(appPersistence::HVSLR_LIGHT_MODE + loggedInUser, lightModeToStore);
}

void SettingsScreenModel::onUserLoggedOutHVSLRRGB(int rgb)
{
    settings.setValue(appPersistence::HVSLR_LIGHT_RGB + loggedInUser, rgb);
}

void SettingsScreenModel::onUserLoggedOutSteamHVSLRRGB(int rgb)
{
    settings.setValue(appPersistence::HVSLR_LIGHT_STEAM_RGB + loggedInUser,rgb);
}

void SettingsScreenModel::onUserLoggedOutLightBrightness(int lightBrightToStore)
{
    settings.setValue(appPersistence::LIGHT_BRIGHTNESS_KEY + loggedInUser, lightBrightToStore);
}

void SettingsScreenModel::onUserLoggedOutLightToggle(bool lightToggleStateToStore)
{
    settings.setValue(appPersistence::LIGHT_TOGGLE_KEY + loggedInUser, lightToggleStateToStore);
}

void SettingsScreenModel::onUserLoggedOutLightColor(int lightColorToStore)
{
    settings.setValue(appPersistence::LIGHT_COLOR_KEY + loggedInUser, lightColorToStore);
}

void SettingsScreenModel::onUserLoggedOutSteamHVSLRLightColor(int steamLightColorToStore)
{
    settings.setValue(appPersistence::HVSLR_STEAM_LIGHT_COLOR_KEY + loggedInUser, steamLightColorToStore);
}

void SettingsScreenModel::onUserLoggedOutSteamHVSLRBrightness(int steamBrightnessToStore)
{
    settings.setValue(appPersistence::HVSL_STEAM_BRIGHTNESS_KEY + loggedInUser,steamBrightnessToStore);
}

void SettingsScreenModel::onUserLoggedOutSteamLightColor(int steamLightColorToStore)
{
    settings.setValue(appPersistence::STEAM_LIGHT_COLOR_KEY + loggedInUser, steamLightColorToStore);
}

void SettingsScreenModel::onUserLoggedOutSteamBrightness(int steamBrightnessToStore)
{
    settings.setValue(appPersistence::STEAM_BRIGHTNESS_KEY + loggedInUser, steamBrightnessToStore);
}

void SettingsScreenModel::onUserLoggedOutHVLSRLightColor(int lightColorToStore)
{
    settings.setValue(appPersistence::HVSLR_LIGHT_COLOR_KEY + loggedInUser, lightColorToStore);
}

void SettingsScreenModel::onSavePreset1(int rgb)
{
    settings.setValue(appPersistence::HVSLR_PRESET_1_COLOR + loggedInUser, rgb);
}

void SettingsScreenModel::onSavePreset2(int rgb)
{
    settings.setValue(appPersistence::HVSLR_PRESET_2_COLOR + loggedInUser, rgb);
}


void SettingsScreenModel::onSavePreset3(int rgb)
{
    settings.setValue(appPersistence::HVSLR_PRESET_3_COLOR + loggedInUser, rgb);
}


void SettingsScreenModel::onSaveWhitePreset1(int rgb)
{
    settings.setValue(appPersistence::HVSLR_PRESET_1_WHITE + loggedInUser, rgb);
}


void SettingsScreenModel::onSaveWhitePreset2(int rgb)
{
    settings.setValue(appPersistence::HVSLR_PRESET_2_WHITE + loggedInUser, rgb);
}


void SettingsScreenModel::onSaveWhitePreset3(int rgb)
{
    settings.setValue(appPersistence::HVSLR_PRESET_3_WHITE + loggedInUser, rgb);
}


void SettingsScreenModel::onTempScaleBtnClicked(int tempScaleAsInt)
{
    TempScale tempScale = static_cast<TempScale>(tempScaleAsInt);

    switch (tempScale) {
        case TempScale::FAHRENHEIT:
            equipmentCanInterface->setTemperatureFormat(0);
            settings.setValue(appPersistence::TEMP_SCALE_KEY + loggedInUser, appPersistence::TEMP_SCALE_F_VALUE);

            break;
        case TempScale::CELCIUS:
            equipmentCanInterface->setTemperatureFormat(1);
            settings.setValue(appPersistence::TEMP_SCALE_KEY + loggedInUser, appPersistence::TEMP_SCALE_C_VALUE);
            break;
    }

    emit tempScaleChanged(settings.value(appPersistence::TEMP_SCALE_KEY + loggedInUser, appPersistence::TEMP_SCALE_DEFAULT_VALUE).toString());
}

void SettingsScreenModel::onTimeFormatBtnClicked(int timeFormatAsInt)
{
    TimeFormat timeFormat = static_cast<TimeFormat>(timeFormatAsInt);

    switch (timeFormat) {
        case TimeFormat::TWELVE_HOUR:
            settings.setValue(appPersistence::TIME_FORMAT_KEY + loggedInUser, appPersistence::TIME_FORMAT_TWELVE_HOUR_VALUE);
            break;
        case TimeFormat::TWENTY_FOUR_HOUR:
            settings.setValue(appPersistence::TIME_FORMAT_KEY + loggedInUser, appPersistence::TIME_FORMAT_TWENTYFOUR_HOUR_VALUE);
            break;
    }

    emit timeFormatChanged(settings.value(appPersistence::TIME_FORMAT_KEY + loggedInUser, appPersistence::TIME_FORMAT_DEFAULT_VALUE).toString());
}

void SettingsScreenModel::onVoiceVolumeSliderChanged(int voiceVolume)
{
    currentVoiceVolumeValue = voiceVolume;
    equipmentCanInterface->changeVoiceVolume(currentVoiceVolumeValue);
    emit voiceVolumeSliderVoiceToggle(true);
}

void SettingsScreenModel::onUserLoggedInVoiceVolume(int voiceVolumeValue)
{
    currentVoiceVolumeValue = voiceVolumeValue;
    equipmentCanInterface->changeVoiceVolume(currentVoiceVolumeValue);
    emit voiceVolumeChanged(currentVoiceVolumeValue);
}

void SettingsScreenModel::onVoiceToggleStateChanged(bool isChecked)
{
    currentVoiceToggleState = isChecked;

    if(!isChecked){
        equipmentCanInterface->turnOnOffVoiceVolume(0);
    }else{
       // equipmentCanInterface->turnOnOffVoiceVolume(1);
        equipmentCanInterface->changeVoiceVolume(0);
    }
}

void SettingsScreenModel::onUserLoggedInVoiceToggle(bool voiceMuteBtnStateValue)
{
    currentVoiceToggleState = voiceMuteBtnStateValue;
    equipmentCanInterface->changeVoiceVolume(0);

    if(!currentVoiceToggleState){
        equipmentCanInterface->turnOnOffVoiceVolume(0);
    }else{
       equipmentCanInterface->turnOnOffVoiceVolume(0);

    }
    emit voiceToggleChanged(currentVoiceToggleState);
}

void SettingsScreenModel::onFastStartChanged(int fastStartValue)
{
    currentFastStartValue = static_cast<FastStart>(fastStartValue);
    equipmentCanInterface->changeFastStartMode(currentFastStartValue);

}

void SettingsScreenModel::onUserLoggedInFastStart(int fastStartValue)
{
    currentFastStartValue = static_cast<FastStart>(fastStartValue);
    equipmentCanInterface->changeFastStartMode(currentFastStartValue);
    emit fastStartChanged(currentFastStartValue);
}

void SettingsScreenModel::onTimeEditChanged(QTime time)
{
    newTime = time;
}

void SettingsScreenModel::onUsernameEntered(QString username)
{
    currentUsername = username;
}

void SettingsScreenModel::onUserIconDownBtnClicked()
{
    currentIconIndex--;
    currentIconIndex = validateUserIconIndex(currentIconIndex);
    emit userIconChanged(USER_ICON_PATHS[currentIconIndex]);
}

void SettingsScreenModel::onUserIconUpBtnClicked()
{
    currentIconIndex++;
    currentIconIndex = validateUserIconIndex(currentIconIndex);
    emit userIconChanged(USER_ICON_PATHS[currentIconIndex]);
}

void SettingsScreenModel::onOrientationChanged(Orientation orientation)
{
    switch (orientation) {
        case Orientation::LANDSCAPE:
           settings.setValue(appPersistence::ORIENTATION_SELECTED_KEY, appPersistence::LANDSCAPE_ORIENTATION_VALUE);
            break;
        case Orientation::PORTRAIT:
           settings.setValue(appPersistence::ORIENTATION_SELECTED_KEY, appPersistence::PORTRAIT_ORIENTATION_VALUE);
            break;
    }

    if(!testBuild::isTestBuild) {
        qApp->quit();
        QProcess::execute(REBOOT_PACKAGE);
    }
}

int SettingsScreenModel::validateUserIconIndex(int index)
{
    if(index < 0) {
        index = 0;
    } else if(index > (USER_ICON_PATHS.size() - 1)) {
        index = USER_ICON_PATHS.size() - 1;
    }

    return index;
}

void SettingsScreenModel::onUserLoggedOut()
{
    emit userLoggedOutVoiceVolume(currentVoiceVolumeValue);
    emit userLoggedOutVoiceToggle(currentVoiceToggleState);
    emit userLoggedOutFastStart((int)currentFastStartValue);
}
