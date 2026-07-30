#ifndef APPCONSTANTS_H
#define APPCONSTANTS_H

#include <QFont>

namespace fontStyle {
    const QString QUICKSAND_BOLD_FAMILY_ID = "quicksand-BoldFamilyId";

    QFont getQuicksandBoldWithSize(int);
}

namespace dialogStyle {
    const QString BUTTON_STYLESHEET =
        "QToolButton {"
            "border: 0;"
            "width: 135px;"
            "height: 47px;"
            "image: url(:/images/common/error_dialog/Error_Dialog_Button_OK_Normal_135x47.png);"
        "}"
        "QToolButton:pressed {"
            "image: url(:/images/common/error_dialog/Error_Dialog_Button_OK_Active_135x47.png);"
        "}";

    const QString DIALOG_STYLESHEET_LANDSCAPE =
        "QMessageBox {"
            "border: 4px solid #FE5000;"
            "background: black;"
            "max-width: 70px;"
            "max-height: 300px;"
        "}"
        "QMessageBox QLabel {"
            "color: white;"
        "}";
    const QString DIALOG_STYLESHEET_PORTRAIT =
        "QMessageBox {"
            "border: 4px solid #FE5000;"
            "background: black;"
            "max-width: 300px;"
            "max-height: 700px;"
        "}"
        "QMessageBox QLabel {"
            "color: white;"
        "}";
}

namespace barQSliderStyleSheet {
    const QString VERTICAL_BAR_QSLIDERS =
        "QSlider::groove:vertical {"
            "border: 2px solid white;"
            "border-radius: 13px;"
            "background: black;"
        "}"
        "QSlider::handle:vertical {"
            "background: white;"
            "height: 23px;"
            "border-radius: 10px;"
        "}"
        "QSlider::add-page:vertical {"
            "background: white;"
            "border-bottom-left-radius: 13px;"
            "border-bottom-right-radius: 13px;"
        "}"
        "QSlider::add-page:vertical[onSliderPressed=false] {"
            "background: white;"
            "border-bottom-left-radius: 13px;"
            "border-bottom-right-radius: 13px;"
        "}"
        "QSlider::add-page:vertical[onSliderPressed=true] {"
            "background: #FE5000;"
            "border-bottom-left-radius: 13px;"
            "border-bottom-right-radius: 13px;"
        "}"
        "QSlider::groove:vertical[onSliderPressed=false] {"
            "border: 2px solid white;"
            "border-radius: 13px;"
            "background: black;"
        "}"
        "QSlider::groove:vertical[onSliderPressed=true] {"
            "border: 2px solid #FE5000;"
            "border-radius: 13px;"
            "background: black;"
        "}"
        "QSlider::handle:vertical[onSliderPressed=false] {"
            "background: white;"
            "height: 23px;"
            "border-radius: 10px;"
        "}"
        "QSlider::handle:vertical[onSliderPressed=true] {"
            "background: #FE5000;"
            "height: 23px;"
            "border-radius: 10px;"
        "}";

    const QString HORIZONTAL_BAR_QSLIDERS =
        "QSlider::groove:horizontal {"
            "border: 2px solid white;"
            "border-radius: 13px;"
            "background: black;"
        "}"
        "QSlider::handle:horizontal {"
            "background: white;"
            "width: 23px;"
            "border-radius: 10px;"
        "}"
        "QSlider::sub-page:horizontal {"
            "background: white;"
            "border-top-left-radius: 13px;"
            "border-bottom-left-radius: 13px;"
        "}"
        "QSlider::sub-page:horizontal[onSliderPressed=false] {"
            "background: white;"
            "border-top-left-radius: 13px;"
            "border-bottom-left-radius: 13px;"
        "}"
        "QSlider::sub-page:horizontal[onSliderPressed=true] {"
            "background: #FE5000;"
            "border-top-left-radius: 13px;"
            "border-bottom-left-radius: 13px;"
        "}"
        "QSlider::groove:horizontal[onSliderPressed=false] {"
            "border: 2px solid white;"
            "border-radius: 13px;"
            "background: black;"
        "}"
        "QSlider::groove:horizontal[onSliderPressed=true] {"
            "border: 2px solid #FE5000;"
            "border-radius: 13px;"
            "background: black;"
        "}"
        "QSlider::handle:horizontal[onSliderPressed=false] {"
            "background: white;"
            "height: 23px;"
            "border-radius: 10px;"
        "}"
        "QSlider::handle:horizontal[onSliderPressed=true] {"
            "background: #FE5000;"
            "height: 23px;"
            "border-radius: 10px;"
        "}";
}

namespace firmware {
    const QString FIRMWARE_VERSION = "0.1.0";
}

namespace appPersistence {

   const QString USER_ONE = "UserOne";
   const QString USER_TWO = "UserTwo";

   const QString ORIENTATION_SELECTED_KEY = "orientationSelected";
   const QString LANDSCAPE_ORIENTATION_VALUE = "landscape";
   const QString PORTRAIT_ORIENTATION_VALUE = "portrait";

   const QString STEMP_TEMP_KEY = "steamTemp";
   const QString STEAM_TIME_KEY = "steamTime";

   const QString WATER_TEMP_KEY = "waterTemp";
   const QString WATER_FLOW_KEY = "waterFlow";

   const QString TIME_FORMAT_KEY = "timeFormat";
   const QString TIME_FORMAT_TWELVE_HOUR_VALUE = "h:mm AP";
   const QString TIME_FORMAT_TWENTYFOUR_HOUR_VALUE = "hh:mm";
   const QString TIME_FORMAT_DEFAULT_VALUE = "h:mm AP";

   const QString TEMP_SCALE_KEY = "tempScale";
   const QString TEMP_SCALE_F_VALUE = "F";
   const QString TEMP_SCALE_C_VALUE = "C";
   const QString TEMP_SCALE_DEFAULT_VALUE = "F";

   const QString MUSIC_TREBLE_KEY = "musicTreble";
   const QString MUSIC_MID_KEY = "musicMid";
   const QString MUSIC_BASS_KEY = "musicBass";

   const QString BT_INPUT_KEY = "btInputToggle";
   const QString AUX_INPUT_KEY = "auxInputToggle";

   const QString VOLUME_KEY = "volume";
   const QString MUTE_SOUND_BTN_KEY = "muteToggle";
   const QString VOICE_VOLUME_KEY = "voiceVolume";
   const QString MUTE_VOICE_KEY = "voiceToggle";

   const QString FAST_START_KEY = "fastStart";

   const QString HVSL_BRIGHTNESS_KEY = "hvslrLightBrightness";
   const QString HVSL_STEAM_BRIGHTNESS_KEY = "hvslrSteamLightBrightness";
   const QString HVSLR_LIGHT_COLOR_KEY = "hvslrLightColor";

   const QString HVSLR_LIGHT_TOGGLE_KEY = "hvslrLightToggle";

   const QString HVSLR_LIGHT_MODE = "hvslrLightMode";

   const QString HVSLR_LIGHT_RGB = "hvslrlightRGB";

   const QString HVSLR_LIGHT_STEAM_RGB = "hvslrlightSteamRGB";

   const QString HVSLR_PRESET_1_COLOR = "hvslrPreset1Color";
   const QString HVSLR_PRESET_2_COLOR = "hvslrPreset2Color";
   const QString HVSLR_PRESET_3_COLOR = "hvslrPreset3Color";
   const QString HVSLR_PRESET_1_WHITE = "hvslrPreset1White";
   const QString HVSLR_PRESET_2_WHITE = "hvslrPreset2White";
   const QString HVSLR_PRESET_3_WHITE = "hvslrPreset3White";

   const QString HVSLR_STEAM_LIGHT_COLOR_KEY = "steamHvslrLightColor";

   const QString LIGHT_BRIGHTNESS_KEY = "lightBrightness";
   const QString LIGHT_TOGGLE_KEY = "lightToggle";
   const QString LIGHT_COLOR_KEY = "lightColor";

   const QString STEAM_LIGHT_COLOR_KEY = "steamLightColor";
   const QString STEAM_BRIGHTNESS_KEY = "steamBrightnessColor";

   const QString USERNAME_KEY = "username";
   const QString USERNAME_ONE_DEFAULT_VALUE = "USER ONE";
   const QString USERNAME_TWO_DEFAULT_VALUE = "USER TWO";

   const QString USERNAME_ICON_KEY = "usernameIcon";
   const QString USERNAME_ONE_ICON_DEFAULT_VALUE = ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Golf_Normal_126x126.png";
   const QString USERNAME_TWO_ICON_DEFAULT_VALUE = ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Dragonfly_Normal_126x126 copy.png";
}

enum class CanDevice {
    DIGITAL_SHOWER = 0,
    STEAM_GENERATOR,
    SLSR,
    HVSLSR
};

enum class CanDeviceState {
    ABSENT = 0,
    PRESENT
};

enum class NavBarButton {
    SHOWER = 0,
    STEAM,
    MUSIC,
    LIGHT,
    SETTINGS,
    POWER
};

enum class NavBarButtonState {
    ABSENT = 0,
    PRESENT
};

enum class PsbState {
    STAND_BY = 0,
    STEAM_ON,
    PF_NEEDED,
    PF_IN_PROGRESS,
    PF_FINISHED,
    ERROR
};

enum class PsbErrorState {
    NO_ERROR = 0,
    OVERTEMP,
    LOW_WATER,
    VOLTAGE_LOW
};

enum class FastStart {
    OFF = 0,
    ON,
    GREEN
};

enum class ShowerValvePowerStatus {
    ON_LINE_POWER = 0,
    ON_BATTERY_POWER,
    BATTERY_POWER_LOW,
    NO_POWER_INFO_FROM_VALVE
};

enum class WaterFlow {
    LOW = 0,
    MEDIUM,
    HIGH
};

enum class WaterOutput {
    RAINHEAD = 0,
    SHOWERHEAD,
    WAND
};

enum class MusicInput {
    NOINPUT = 0,
    BLUETOOTH,
    AUX
};

enum class VoiceChip {
    NOVOICECHIP = 0,
    VOICECHIP
};

enum class BluetoothState {
    BLUETOOTH_SEARCHING = 0,
    PAIRING = 1,
    PAIRED = 2,
    NO_BLUETOOTH_HARDWARE = 9
};

enum class BluetoothEquipment{
    BT_EQUIPMENT_UNAVAILABLE = 0,
    BT_EQUIPMENT_AVAILABLE
};

enum class HvslrSource{
    FM = 0,
    BT,
    AUX,
    VOICE
};

enum class Light {
    VIOLET = 0,
    INDIGO,
    BLUE,
    GREEN,
    YELLOW,
    ORANGE,
    RED,
    MOOD,
    WHITE,
    CHROMA,
    COLOR
};


enum class User {
    USER_ONE = 0,
    USER_TWO
};

enum class SettingsScreenAll {
    GENERAL = 0,
    TEMP_SCALE,
    TIME_FORMAT,
    SET_TIME,
    USERNAMES,
    EQUIPMENT,
    ORIENTATION,
    REBOOT,
    ABOUT,
    STEAM_GENERATOR,
    DIGITAL_SHOWER,
    SLSR,
    DIAGNOSTICS,
    KEYBOARD
};

enum class TempScale {
    FAHRENHEIT = 0,
    CELCIUS
};

enum class TimeFormat {
    TWELVE_HOUR = 0,
    TWENTY_FOUR_HOUR
};

enum class Orientation {
    LANDSCAPE = 0,
    PORTRAIT
};

#endif // APPCONSTANTS_H
