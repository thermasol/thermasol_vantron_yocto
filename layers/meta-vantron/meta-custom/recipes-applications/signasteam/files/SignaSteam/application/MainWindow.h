#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AppConstants.h"

#include <QLabel>
#include <QMainWindow>
#include <QSlider>
#include <QMap>
#include <QSettings>
#include <QStackedWidget>
#include <QToolButton>
#include <QObject>
#include <QWidgetList>
#include <QEvent>
#include <QTimer>

namespace Ui {
    class MainWindowLandscape;
    class MainWindowPortrait;
}

class EquipmentCanInterface;
class BluetoothCanInterface;
class SteamCanInterface;
class ShowerCanInterface;
class MusicCanInterface;
class LightCanInterface;
class HVSLRLightCanInterface;
class MainWindowModel;
class SteamScreenModel;
class ShowerScreenModel;
class MusicScreenModel;
class LightScreenModel;
class HVSLRLightScreenModel;
class SettingsScreenModel;
class UserScreen;
class SteamScreen;
class ShowerScreen;
class MusicScreen;
class LightScreen;
class HVSLRLightScreen;
class SettingsScreen;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void userLoggedOut();
    void settingScreenChanged(SettingsScreenAll);
    void screenTimeout(int);
    void turnOffEzs();

public slots:
    void onScreenTimeout();
    void onPowerFlushInProgress(bool);
    void onVolumeChanged(int);
    void onMuteToggleChanged(bool);
    void onUsernameChanged(QString);
    void onClockTimeChanged(QString);
    void onNavBarChanged(QMap<NavBarButton, NavBarButtonState>);
    void onUserLoggedIn();
    void onVolumeSliderMuteToggleChanged (bool);
    void handleScreenTimeoutSteam(bool);

private slots:
    void onNavBarButtonClicked(int);
    void onSliderPressed();
    void onSliderReleased();
    void onBacklightOffBtnClicked();

private:
    const int SLSR_VOLUME_DEFAULT_VALUE = 10;
    const int MAIN_WINDOW_TEXT_FONT_SIZE = 32;
    const QString BACKLIGHT_SYS_CLASS = "/sys/class/backlight/backlight/brightness";
    const QString BACKLIGHT_OFF_VALUE = "0";
    const QString BACKLIGHT_ON_VALUE = "6";
    const QList<QString> NAV_BAR_BTN_IDS = {
        "showerScreenButton",
        "steamScreenButton",
        "musicScreenButton",
        "lightScreenButton",
        "settingsScreenButton",
        "powerScreenButton"
    };
    const QList<QPoint> ICON_LOCATIONS_ONE_LAND = {
        QPoint(364, 3)
    };
    const QList<QPoint> ICON_LOCATIONS_TWO_LAND = {
        QPoint(296, 3),
        QPoint(415, 3)
    };
    const QList<QPoint> ICON_LOCATIONS_THREE_LAND = {
        QPoint(248, 3),
        QPoint(357, 3),
        QPoint(477, 3)
    };
    const QList<QPoint> ICON_LOCATIONS_FOUR_LAND = {
        QPoint(221, 3),
        QPoint(317, 3),
        QPoint(409, 3),
        QPoint(499, 3)
    };
    const QList<QPoint> ICON_LOCATIONS_ONE_PORT = {
        QPoint(0, 260)
    };
    const QList<QPoint> ICON_LOCATIONS_TWO_PORT = {
        QPoint(0, 169),
        QPoint(0, 370)
    };
    const QList<QPoint> ICON_LOCATIONS_THREE_PORT = {
        QPoint(0, 78),
        QPoint(0, 279),
        QPoint(0, 461)
    };
    const QList<QPoint> ICON_LOCATIONS_FOUR_PORT = {
        QPoint(0, 55),
        QPoint(0, 203),
        QPoint(0, 343),
        QPoint(0, 471)
    };
    Ui::MainWindowLandscape *uiLandscape;
    Ui::MainWindowPortrait *uiPortrait;
    EquipmentCanInterface *equipmentCanInterface;
    BluetoothCanInterface *bluetoothCanInterface;
    SteamCanInterface *steamCanInterface;
    ShowerCanInterface *showerCanInterface;
    MusicCanInterface *musicCanInterface;
    LightCanInterface *lightCanInterface;
    HVSLRLightCanInterface *hvslrLightCanInterface;
    MainWindowModel *mainWindowModel;
    SteamScreenModel *steamScreenModel;
    ShowerScreenModel *showerScreenModel;
    MusicScreenModel *musicScreenModel;
    LightScreenModel *lightScreenModel;
    HVSLRLightScreenModel *hvslrLightScreenModel;
    SettingsScreenModel *settingsScreenModel;
    UserScreen *userScreen;
    SteamScreen *steamScreen;
    ShowerScreen *showerScreen;
    MusicScreen *musicScreen;
    LightScreen *lightScreen;
    HVSLRLightScreen *hvslrLightScreen;
    SettingsScreen *settingsScreen;
    QButtonGroup* navBarGroup;
    QFrame *navBarFrame;
    QLabel *usernameValue;
    QLabel *clockTimeValue;
    QLabel *navBarActiveContainerSeparator;
    QLabel *thermasolLogoUserScreen;
    QLabel *thermasolLogoMainWindow;
    QSlider *volumeBarSlider;
    QStackedWidget *activeScreenContainer;
    QSettings settings;
    QString orientationSelected;
    QToolButton *volumeMuteToggleBtn;
    QToolButton *powerButton;
    QToolButton *backlightOffBtn;
    QToolButton *steamScreenButton;
    QToolButton *showerScreenButton;
    QToolButton *musicScreenButton;
    QToolButton *lightScreenButton;
    QToolButton* settingsScreenButton;
    QFrame *timeAndUserFrame;
    QTimer *screenTimeoutTimer;
    bool steamOn;
    void dimScreen();
    void turnOnScreen();
    void moveUserToSettingsScreen();
    void moveUserToSteamScreen();
    void moveToUserScreen();
    void showDialog(QString, QString);
    void resetScreenTimeout();
};

#endif // MAINWINDOW_H
