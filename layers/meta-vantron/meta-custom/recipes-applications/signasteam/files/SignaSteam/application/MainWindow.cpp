#include "MainWindow.h"
#include "ui_MainWindow-portrait.h"
#include "ui_MainWindow-landscape.h"

#include "MainWindowModel.h"
#include "login/UserScreen.h"
#include "steam_control/SteamCanInterface.h"
#include "steam_control/SteamScreenModel.h"
#include "steam_control/SteamScreen.h"
#include "shower_control/ShowerCanInterface.h"
#include "shower_control/ShowerScreenModel.h"
#include "shower_control/ShowerScreen.h"
#include "music_control/MusicCanInterface.h"
#include "music_control/MusicScreenModel.h"
#include "music_control/MusicScreen.h"
#include "light_control/LightCanInterface.h"
#include "light_control/LightScreenModel.h"
#include "light_control/LightScreen.h"
#include "hvslr_light_control/HVSLRLightCanInterface.h"
#include "hvslr_light_control/HVSLRLightScreenModel.h"
#include "hvslr_light_control/HVSLRLightScreen.h"
#include "settings/SettingsScreenModel.h"
#include "settings/SettingsScreen.h"
#include "settings_equipment/EquipmentCanInterface.h"
#include "settings_equipment/BluetoothCanInterface.h"

#include <QFile>
#include <QMessageBox>
#include <QStyle>
#include <QEvent>
#include <QWidgetList>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QSettings>

#include <settings/SettingsScreenModel.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    equipmentCanInterface(new EquipmentCanInterface()),
    bluetoothCanInterface(new BluetoothCanInterface()),
    steamCanInterface(new SteamCanInterface()),
    showerCanInterface(new ShowerCanInterface()),
    musicCanInterface(new MusicCanInterface(this, bluetoothCanInterface)),
    lightCanInterface(new LightCanInterface()),
    hvslrLightCanInterface(new HVSLRLightCanInterface()),
    mainWindowModel(new MainWindowModel(this, equipmentCanInterface)),
    steamScreenModel(new SteamScreenModel(this, steamCanInterface)),
    showerScreenModel(new ShowerScreenModel(this, showerCanInterface)),
    musicScreenModel(new MusicScreenModel(this, musicCanInterface)),
    lightScreenModel(new LightScreenModel(this, lightCanInterface)),
    hvslrLightScreenModel(new HVSLRLightScreenModel(this, hvslrLightCanInterface)),
    settingsScreenModel(new SettingsScreenModel(this)),
    userScreen(new UserScreen(this)),
    steamScreen(new SteamScreen(this, steamScreenModel)),
    showerScreen(new ShowerScreen(this, showerScreenModel)),
    musicScreen(new MusicScreen(this, musicScreenModel)),
    lightScreen(new LightScreen(this, lightScreenModel)),
    hvslrLightScreen(new HVSLRLightScreen(this, hvslrLightScreenModel)),
    settingsScreen(new SettingsScreen(this, settingsScreenModel, equipmentCanInterface, bluetoothCanInterface))
{
    //Send back-light brightness to Signatouch device on app launch.
    QFile backlightFile(BACKLIGHT_SYS_CLASS);
    if (backlightFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream commandTextStream(&backlightFile);
        QString command(BACKLIGHT_ON_VALUE);
        commandTextStream << command;
        backlightFile.close();
    }

    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::MainWindowLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::MainWindowPortrait;
        uiPortrait->setupUi(this);

        timeAndUserFrame = findChild<QFrame*>("timeAndUserFrame");
        timeAndUserFrame->hide();
    }

    //Send fast start mode on app launch.
    equipmentCanInterface->changeFastStartMode(FastStart::ON); //TODO Find out a way to where app persistence is used to set default


    screenTimeoutTimer = new QTimer();
    connect(screenTimeoutTimer, &QTimer::timeout, this, &MainWindow::onScreenTimeout);
    screenTimeoutTimer->start(600000);

    clockTimeValue = findChild<QLabel*>("clockTimeValue");
    clockTimeValue->setFont(fontStyle::getQuicksandBoldWithSize(MAIN_WINDOW_TEXT_FONT_SIZE));
    connect(mainWindowModel, &MainWindowModel::clockTimeChanged, this, &MainWindow::onClockTimeChanged);

    usernameValue = findChild<QLabel*>("usernameValue");
    usernameValue->setFont(fontStyle::getQuicksandBoldWithSize(MAIN_WINDOW_TEXT_FONT_SIZE));
    connect(mainWindowModel, &MainWindowModel::usernameChanged, this, &MainWindow::onUsernameChanged);
    connect(settingsScreenModel, &SettingsScreenModel::usernameChanged, mainWindowModel, &MainWindowModel::usernameChanged);

    navBarGroup = findChild<QButtonGroup*>("navBarButtonGroup");
    connect(mainWindowModel, &MainWindowModel::navBarChanged, this, &MainWindow::onNavBarChanged);
    connect(navBarGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onNavBarButtonClicked);

    steamScreenButton = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(NavBarButton::STEAM)]);
    steamScreenButton->hide();
    navBarGroup->setId(steamScreenButton, static_cast<int>(NavBarButton::STEAM));
    connect(steamScreen, &SteamScreen::pfBtnClicked, this, &MainWindow::moveUserToSettingsScreen);
    connect(steamScreen, &SteamScreen::pfBtnClicked, settingsScreen, &SettingsScreen::onPfBtnClicked);


    showerScreenButton = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(NavBarButton::SHOWER)]);
    showerScreenButton->hide();
    navBarGroup->setId(showerScreenButton, static_cast<int>(NavBarButton::SHOWER));

    musicScreenButton = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(NavBarButton::MUSIC)]);
    musicScreenButton->hide();
    navBarGroup->setId(musicScreenButton, static_cast<int>(NavBarButton::MUSIC));

    lightScreenButton = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(NavBarButton::LIGHT)]);
    lightScreenButton->hide();
    navBarGroup->setId(lightScreenButton, static_cast<int>(NavBarButton::LIGHT));

    settingsScreenButton = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(NavBarButton::SETTINGS)]);
    navBarGroup->setId(settingsScreenButton, static_cast<int>(NavBarButton::SETTINGS));
    connect(this, &MainWindow::settingScreenChanged, settingsScreen, &SettingsScreen::onSettingsScreenChanged);

    powerButton = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(NavBarButton::POWER)]);
    navBarGroup->setId(powerButton, static_cast<int>(NavBarButton::POWER));

    navBarFrame = findChild<QFrame*>("navBarFrame");
    navBarFrame->raise();

    navBarActiveContainerSeparator = findChild<QLabel*>("navBarActiveContainerSeparator");
    navBarActiveContainerSeparator->hide();

    activeScreenContainer = findChild<QStackedWidget*>("activeScreenContainer");
    activeScreenContainer->addWidget(userScreen);
    activeScreenContainer->addWidget(steamScreen);
    activeScreenContainer->addWidget(showerScreen);
    activeScreenContainer->addWidget(musicScreen);
    activeScreenContainer->addWidget(lightScreen);
    activeScreenContainer->addWidget(settingsScreen);
    activeScreenContainer->addWidget(hvslrLightScreen);

    connect(equipmentCanInterface, &EquipmentCanInterface::powerFlushStarted, this, &MainWindow::onPowerFlushInProgress);

    connect(userScreen, &UserScreen::userLoggedIn, this, &MainWindow::onUserLoggedIn);
    connect(userScreen, &UserScreen::userLoggedIn, settingsScreenModel, &SettingsScreenModel::onUserLoggedIn);
    connect(settingsScreenModel, &SettingsScreenModel::refreshUserScreen, userScreen, &UserScreen::onRefreshUserScreen);
    connect(this, &MainWindow::turnOffEzs, mainWindowModel, &MainWindowModel::onEzsTurnOff);

    connect(settingsScreenModel, &SettingsScreenModel::timeChanged, mainWindowModel, &MainWindowModel::onTimeChanged);
    connect(settingsScreenModel, &SettingsScreenModel::timeFormatChanged, mainWindowModel, &MainWindowModel::onTimeFormatChanged);
    connect(settingsScreenModel, &SettingsScreenModel::tempScaleChanged, steamScreenModel, &SteamScreenModel::onTempScaleChanged);

    connect(steamScreenModel, &SteamScreenModel::userLoggedOutSteamTemp, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamTemp);
    connect(steamScreenModel, &SteamScreenModel::userLoggedOutSteamTime, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamTime);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInSteamTemp, steamScreenModel, &SteamScreenModel::onUserLoggedInSteamTemp);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInSteamTime, steamScreenModel, &SteamScreenModel::onUserLoggedInSteamTime);
    connect(equipmentCanInterface, &EquipmentCanInterface::steamStarted, this, &MainWindow::handleScreenTimeoutSteam);

    connect(userScreen, &UserScreen::ezsExit, this, &MainWindow::moveToUserScreen, Qt::UniqueConnection);
    connect(equipmentCanInterface, &EquipmentCanInterface::ezsSteamOffFlag, steamScreen, &SteamScreen::onStopSessionClicked, Qt::UniqueConnection);
    connect(equipmentCanInterface, &EquipmentCanInterface::ezsSteamOffFlag, steamScreenModel, &SteamScreenModel::onStopSessionClicked, Qt::UniqueConnection);
    connect(equipmentCanInterface, &EquipmentCanInterface::ezsSteamOnFlag, steamScreenModel, &SteamScreenModel::onSlideToStartSwiped, Qt::UniqueConnection);
    connect(equipmentCanInterface, &EquipmentCanInterface::ezsSteamOnFlag, steamScreen, &SteamScreen::onSlideToStartSwiped, Qt::UniqueConnection);
    connect(equipmentCanInterface, &EquipmentCanInterface::ezsSteamOnFlag, this, &MainWindow::moveUserToSteamScreen, Qt::UniqueConnection);


    connect(musicScreenModel, &MusicScreenModel::userLoggedOutTreble, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutTreble);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInTreble, musicScreenModel, &MusicScreenModel::onUserLoggedInTreble);
    connect(musicScreenModel, &MusicScreenModel::userLoggedOutMid, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutMid);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInMid, musicScreenModel, &MusicScreenModel::onUserLoggedInMid);
    connect(musicScreenModel, &MusicScreenModel::userLoggedOutBass, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutBass);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInBass, musicScreenModel, &MusicScreenModel::onUserLoggedInBass);

    connect(musicScreenModel, &MusicScreenModel::userLoggedOutBtToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutBtToggle);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInBtToggle, musicScreenModel, &MusicScreenModel::onUserLoggedInBtToggle);
    connect(musicScreenModel, &MusicScreenModel::userLoggedOutAuxToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutAuxToggle);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInAuxToggle, musicScreenModel, &MusicScreenModel::onUserLoggedInAuxToggle);
    connect(equipmentCanInterface, &EquipmentCanInterface::showModeState, musicScreenModel, &MusicScreenModel::onShowModeChecked);

    //HVSLSR Light brightness
    //Save on logout
    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutLightBrightness, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutHVSLRLightBrightness);
    //restore on Login
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInHVSLRLightBrightness, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInLightBrightness);

    //HVSLR Light Toggle
    //save on logout
    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutLightToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutHVSLRLightToggle);
    //restor on login
    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRLightToggle, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInLightToggle);

    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutColorMode, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutHVSLRLightMode);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInHVSLRLightMode, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInLightMode);

    //HVSLR color on logging
    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutRGB, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutHVSLRRGB);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInHVSLRRGB, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInRGB);


    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInHVSLRLightColor,hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInLightColor);
    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutLightColor,settingsScreenModel, &SettingsScreenModel::onUserLoggedOutHVLSRLightColor);

    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRPreset1Color,hvslrLightScreenModel,&HVSLRLightScreenModel::onUserLoggedInPreset1Color);
    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRPreset2Color,hvslrLightScreenModel,&HVSLRLightScreenModel::onUserLoggedInPreset2Color);
    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRPreset3Color,hvslrLightScreenModel,&HVSLRLightScreenModel::onUserLoggedInPreset3Color);
    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRPreset1White,hvslrLightScreenModel,&HVSLRLightScreenModel::onUserLoggedInPreset1White);
    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRPreset2White,hvslrLightScreenModel,&HVSLRLightScreenModel::onUserLoggedInPreset2White);
    connect(settingsScreenModel,&SettingsScreenModel::userLoggedInHVSLRPreset3White,hvslrLightScreenModel,&HVSLRLightScreenModel::onUserLoggedInPreset3White);

    connect(hvslrLightScreen,&HVSLRLightScreen::savePresetColor1, settingsScreenModel, &SettingsScreenModel::SettingsScreenModel::onSavePreset1);
    connect(hvslrLightScreen,&HVSLRLightScreen::savePresetColor2, settingsScreenModel, &SettingsScreenModel::SettingsScreenModel::onSavePreset2);
    connect(hvslrLightScreen,&HVSLRLightScreen::savePresetColor3, settingsScreenModel, &SettingsScreenModel::SettingsScreenModel::onSavePreset3);

    connect(hvslrLightScreen,&HVSLRLightScreen::savePresetWhite1, settingsScreenModel, &SettingsScreenModel::SettingsScreenModel::onSaveWhitePreset1);
    connect(hvslrLightScreen,&HVSLRLightScreen::savePresetWhite2, settingsScreenModel, &SettingsScreenModel::SettingsScreenModel::onSaveWhitePreset2);
    connect(hvslrLightScreen,&HVSLRLightScreen::savePresetWhite3, settingsScreenModel, &SettingsScreenModel::SettingsScreenModel::onSaveWhitePreset3);


    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutSteamLightColor, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamHVSLRLightColor);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInHVSLRSteamLightColor, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInSteamLightColor);

    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutSteamBrightness, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamHVSLRBrightness);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInHVSLRSteamBrightness, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInSteamLightBrightness);


    connect(hvslrLightScreenModel, &HVSLRLightScreenModel::userLoggedOutSteamRGB, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamHVSLRRGB);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInSteamHVSLRRGB, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedInSteamRGB);

    connect(equipmentCanInterface, &EquipmentCanInterface::steamStarted, hvslrLightScreenModel, &HVSLRLightScreenModel::onSteamLightOn);



    connect(lightScreenModel, &LightScreenModel::userLoggedOutLightBrightness, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutLightBrightness);

    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInLightBrightness, lightScreenModel, &LightScreenModel::onUserLoggedInLightBrightness);
    connect(lightScreenModel, &LightScreenModel::userLoggedOutLightToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutLightToggle);

    connect(lightScreenModel, &LightScreenModel::userLoggedOutLightColor, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutLightColor);

    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInLightColor, lightScreenModel, &LightScreenModel::onUserLoggedInLightColor);

    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInLightToggle, lightScreenModel, &LightScreenModel::onUserLoggedInLightToggle);


    connect(lightScreenModel, &LightScreenModel::userLoggedOutSteamLightColor, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamLightColor);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInSteamLightColor, lightScreenModel, &LightScreenModel::onUserLoggedInSteamLightColor);
    connect(lightScreenModel, &LightScreenModel::userLoggedOutSteamBrightness, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutSteamBrightness);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInSteamBrightness, lightScreenModel, &LightScreenModel::onUserLoggedInSteamLightBrightness);

    connect(equipmentCanInterface, &EquipmentCanInterface::steamStarted, lightScreenModel, &LightScreenModel::onSteamLightOn);



    connect(mainWindowModel, &MainWindowModel::userLoggedOutVolume, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutVolume);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInVolume, mainWindowModel, &MainWindowModel::onUserLoggedInVolume);
    connect(mainWindowModel, &MainWindowModel::userLoggedOutMuteToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutMuteToggle);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInMuteToggle, mainWindowModel, &MainWindowModel::onUserLoggedInMuteToggle);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedOutVoiceVolume, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutVoiceVolume);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInVoiceVolume, settingsScreenModel, &SettingsScreenModel::onUserLoggedInVoiceVolume);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedOutVoiceToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutVoiceToggle);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInVoiceToggle, settingsScreenModel, &SettingsScreenModel::onUserLoggedInVoiceToggle);

    connect(settingsScreenModel, &SettingsScreenModel::userLoggedOutFastStart, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutFastStart);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInFastStart, settingsScreenModel, &SettingsScreenModel::onUserLoggedInFastStart);

    connect(settingsScreenModel, &SettingsScreenModel::tempScaleChanged, showerScreenModel, &ShowerScreenModel::onTempScaleChanged);
    connect(showerScreenModel, &ShowerScreenModel::userLoggedOutWaterTemp, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutWaterTemp);
    connect(showerScreenModel, &ShowerScreenModel::userLoggedOutWaterFlow, settingsScreenModel, &SettingsScreenModel::onUserLoggedOutWaterFlow);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInWaterTemp, showerScreenModel, &ShowerScreenModel::onUserLoggedInWaterTemp);
    connect(settingsScreenModel, &SettingsScreenModel::userLoggedInWaterFlow, showerScreenModel, &ShowerScreenModel::onUserLoggedInWaterFlow);

    volumeBarSlider = findChild<QSlider*>("volumeBarSlider");
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        volumeBarSlider->setStyleSheet(barQSliderStyleSheet::VERTICAL_BAR_QSLIDERS);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        volumeBarSlider->setStyleSheet(barQSliderStyleSheet::HORIZONTAL_BAR_QSLIDERS);
    }
    connect(volumeBarSlider, &QSlider::valueChanged, mainWindowModel, &MainWindowModel::onVolumeBarSliderChanged);
    connect(volumeBarSlider, &QSlider::sliderPressed, this, &MainWindow::onSliderPressed);
    connect(volumeBarSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    connect(mainWindowModel, &MainWindowModel::volumeValueChanged, this, &MainWindow::onVolumeChanged);
    volumeBarSlider->hide();


    volumeMuteToggleBtn = findChild<QToolButton*>("volumeMuteToggleBtn");
    connect(volumeMuteToggleBtn, &QToolButton::toggled, mainWindowModel, &MainWindowModel::onMuteToggleStateChanged);
    connect(mainWindowModel, &MainWindowModel::muteToggleChanged, this, &MainWindow::onMuteToggleChanged);
    connect(mainWindowModel, &MainWindowModel::volumeSliderMuteToggleChanged, this, &MainWindow::onVolumeSliderMuteToggleChanged);
    volumeMuteToggleBtn->hide();

    thermasolLogoMainWindow = findChild<QLabel*>("thermasolLogoMainWindow");


    backlightOffBtn = findChild<QToolButton*>("backlightOffBtnClickArea");
    backlightOffBtn->hide();
    connect(backlightOffBtn, &QToolButton::clicked, this, &MainWindow::onBacklightOffBtnClicked);

    thermasolLogoUserScreen = findChild<QLabel*>("thermasolLogoUserScreen");
    thermasolLogoUserScreen->raise();

    connect(this, &MainWindow::userLoggedOut, hvslrLightScreen, &HVSLRLightScreen::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, hvslrLightScreenModel, &HVSLRLightScreenModel::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, settingsScreenModel, &SettingsScreenModel::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, steamScreenModel, &SteamScreenModel::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, showerScreenModel, &ShowerScreenModel::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, musicScreenModel, &MusicScreenModel::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, lightScreen, &LightScreen::onUserLoggedOut);
    connect(this, &MainWindow::userLoggedOut, lightScreenModel, &LightScreenModel::onUserLoggedOut);

    connect(this, &MainWindow::userLoggedOut, settingsScreen, &SettingsScreen::userLoggedOut);
    connect(this, &MainWindow::userLoggedOut, mainWindowModel, &MainWindowModel::onUserLoggedOut);

    connect(equipmentCanInterface, &EquipmentCanInterface::psbStateChanged, steamScreen, &SteamScreen::onPsbStateChanged);
}

MainWindow::~MainWindow()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void MainWindow::onPowerFlushInProgress(bool isInProgress){
    if(isInProgress){
        screenTimeoutTimer->stop();
        powerButton->hide();
        settingsScreenButton->hide();
        steamScreenButton->hide();
        musicScreenButton->hide();
        showerScreenButton->hide();
        lightScreenButton->hide();
    }else{
        powerButton->show();
        settingsScreenButton->show();
    }
}

void MainWindow::handleScreenTimeoutSteam(bool isSteamActive){
    steamOn = isSteamActive;

    if(isSteamActive){
        screenTimeoutTimer->stop();
    }else{
        screenTimeoutTimer->start();
    }
}

void MainWindow::onVolumeChanged(int volumeValue)
{
    volumeBarSlider->setValue(volumeValue);
}

void MainWindow::onMuteToggleChanged(bool muteBtnChecked)
{
    volumeMuteToggleBtn->setChecked(muteBtnChecked);
}

void MainWindow::onVolumeSliderMuteToggleChanged(bool sliderHasMoved)
{
    if(sliderHasMoved){
        volumeMuteToggleBtn->setChecked(true);
    }
}

void MainWindow::onUsernameChanged(QString username)
{
    usernameValue->setText(username);
}

void MainWindow::onClockTimeChanged(QString clockTime)
{
    clockTimeValue->setText(clockTime);
}

void MainWindow::onNavBarChanged(QMap<NavBarButton, NavBarButtonState> currentNavBarButtons)
{
    int visibleButtonsCount = 0;
    QMapIterator<NavBarButton, NavBarButtonState> i(currentNavBarButtons);
    while (i.hasNext()) {
        i.next();
        if(i.value() == NavBarButtonState::PRESENT) {
            visibleButtonsCount++;
        }
    }

    QList<QPoint> newIconLocations;
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        switch (visibleButtonsCount) {
            case 1:
                newIconLocations = ICON_LOCATIONS_ONE_LAND;
                break;
            case 2:
                newIconLocations = ICON_LOCATIONS_TWO_LAND;
                break;
            case 3:
                newIconLocations = ICON_LOCATIONS_THREE_LAND;
                break;
            case 4:
                newIconLocations = ICON_LOCATIONS_FOUR_LAND;
                break;
        }
    } else if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
        switch (visibleButtonsCount) {
            case 1:
                newIconLocations = ICON_LOCATIONS_ONE_PORT;
                break;
            case 2:
                newIconLocations = ICON_LOCATIONS_TWO_PORT;
                break;
            case 3:
                newIconLocations = ICON_LOCATIONS_THREE_PORT;
                break;
            case 4:
                newIconLocations = ICON_LOCATIONS_FOUR_PORT;
                break;
        }
    }

    int iconLocationArrayIndex = 0;
    i.toFront();
    while (i.hasNext()) {
        i.next();
        if(i.value() == NavBarButtonState::PRESENT) {
            if(i.key() == NavBarButton::MUSIC || i.key() == NavBarButton::LIGHT) {
                thermasolLogoMainWindow->hide();
                volumeBarSlider->show();
                volumeMuteToggleBtn->show();
            }

            QToolButton* iconToUpdate = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(i.key())]);
            iconToUpdate->move(newIconLocations[iconLocationArrayIndex]);
            iconToUpdate->show();
            iconLocationArrayIndex++;
        } else if (i.value() == NavBarButtonState::ABSENT) {
            switch(i.key()) {
                case NavBarButton::SHOWER:
                    if(activeScreenContainer->currentWidget()->objectName() == "ShowerScreenLandscape"
                            || activeScreenContainer->currentWidget()->objectName() == "ShowerScreenPortrait") {
                        moveUserToSettingsScreen();
                    }
                    showerScreenModel->onEquipmentDisconnected();
                    break;
                case NavBarButton::STEAM:
                    if(activeScreenContainer->currentWidget()->objectName() == "SteamScreenLandscape"
                            || activeScreenContainer->currentWidget()->objectName() == "SteamScreenPortrait") {
                        moveUserToSettingsScreen();
                    }
                    steamScreenModel->onEquipmentDisconnected();
                    break;
                case NavBarButton::MUSIC:
                    if(activeScreenContainer->currentWidget()->objectName() == "MusicScreenLandscape"
                            || activeScreenContainer->currentWidget()->objectName() == "MusicScreenPortrait") {
                        moveUserToSettingsScreen();
                    }

                    thermasolLogoMainWindow->show();
                    volumeBarSlider->hide();
                    volumeMuteToggleBtn->hide();
                    break;
                case NavBarButton::LIGHT:
                    if(mainWindowModel->isHVSLR())
                    {
                        if(activeScreenContainer->currentWidget()->objectName() == "HVSLRLightScreenLandscape"
                                || activeScreenContainer->currentWidget()->objectName()== "HVSLRLightScreenPortrait") {
                            moveUserToSettingsScreen();
                        }
                    }
                    else
                    {
                        if(activeScreenContainer->currentWidget()->objectName() == "LightScreenLandscape"
                                || activeScreenContainer->currentWidget()->objectName() == "LightScreenPortrait") {
                            moveUserToSettingsScreen();
                        }
                    }

                    thermasolLogoMainWindow->show();
                    volumeBarSlider->hide();
                    volumeMuteToggleBtn->hide();
                    break;
            }

            QToolButton* iconToUpdate = findChild<QToolButton*>(NAV_BAR_BTN_IDS[static_cast<int>(i.key())]);
            iconToUpdate->hide();
        }
    }
    equipmentCanInterface->voiceMute();
}



void MainWindow::onUserLoggedIn()
{
    equipmentCanInterface->voiceMute();
    if(!showerScreenButton->isHidden()) {
        onNavBarButtonClicked(static_cast<int>(NavBarButton::SHOWER));
        showerScreenButton->setChecked(true);
    } else if(!steamScreenButton->isHidden()) {
        onNavBarButtonClicked(static_cast<int>(NavBarButton::STEAM));
        steamScreenButton->setChecked(true);
    }  else if (!musicScreenButton->isHidden() || !lightScreenButton->isHidden()) {
        onNavBarButtonClicked(static_cast<int>(NavBarButton::MUSIC));
        musicScreenButton->setChecked(true);
    } else {
        onNavBarButtonClicked(static_cast<int>(NavBarButton::SETTINGS));
        settingsScreenButton->setChecked(true);
    }
}

void MainWindow::onNavBarButtonClicked(int buttonClickedIdAsInt)
{
    NavBarButton buttonClickedId = static_cast<NavBarButton>(buttonClickedIdAsInt);

    switch (buttonClickedId) {
        case NavBarButton::STEAM:
            if(!steamOn){
                resetScreenTimeout();
            }
            activeScreenContainer->setCurrentWidget(steamScreen);
            activeScreenContainer->lower();
            navBarFrame->lower();
            navBarActiveContainerSeparator->show();
            thermasolLogoUserScreen->hide();
            if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                timeAndUserFrame->show();
            }
            break;
        case NavBarButton::SHOWER:
            if(!steamOn){
                resetScreenTimeout();
            }
            activeScreenContainer->setCurrentWidget(showerScreen);
            activeScreenContainer->lower();
            navBarFrame->lower();
            navBarActiveContainerSeparator->show();
            thermasolLogoUserScreen->hide();
            if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                timeAndUserFrame->show();
            }
            break;
        case NavBarButton::MUSIC:
            if(!steamOn){
                resetScreenTimeout();
            }
            activeScreenContainer->setCurrentWidget(musicScreen);
            activeScreenContainer->lower();
            navBarFrame->lower();
            navBarActiveContainerSeparator->show();
            thermasolLogoUserScreen->hide();
            if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                timeAndUserFrame->show();
            }
            break;
        case NavBarButton::LIGHT:
            if(!steamOn){
                resetScreenTimeout();
            }
            if(mainWindowModel->isHVSLR())
            {
                activeScreenContainer->setCurrentWidget(hvslrLightScreen);
            } else {
                activeScreenContainer->setCurrentWidget(lightScreen);
            }

            activeScreenContainer->lower();
            navBarFrame->lower();
            navBarActiveContainerSeparator->show();
            thermasolLogoUserScreen->hide();
            if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                timeAndUserFrame->show();
            }
            break;
        case NavBarButton::SETTINGS:
            if(!steamOn){
                resetScreenTimeout();
            }
            activeScreenContainer->setCurrentWidget(settingsScreen);
            emit settingScreenChanged(SettingsScreenAll::GENERAL);
            activeScreenContainer->raise();
            navBarFrame->lower();
            navBarActiveContainerSeparator->show();
            thermasolLogoUserScreen->hide();
            if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                timeAndUserFrame->show();
            }
            break;
        case NavBarButton::POWER:
            activeScreenContainer->setCurrentWidget(userScreen);
            activeScreenContainer ->raise();
            navBarFrame->raise();
            navBarActiveContainerSeparator->hide();
            thermasolLogoUserScreen->show();
            thermasolLogoUserScreen->raise();
            if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                timeAndUserFrame->hide();
            }
          //  equipmentCanInterface->turnOnOffVoiceVolume(0);
            emit userLoggedOut();
            emit turnOffEzs();

            dimScreen();

            navBarGroup->setExclusive(false);
            powerButton->setChecked(false);
            navBarGroup->setExclusive(true);


            break;
    }
}

void MainWindow::onScreenTimeout(){

    //dimScreen();
}

void MainWindow::resetScreenTimeout(){
    screenTimeoutTimer->stop();
    screenTimeoutTimer->start(600000);
}

void MainWindow::onBacklightOffBtnClicked()
{
    turnOnScreen();
}

void MainWindow::turnOnScreen()
{
    backlightOffBtn->hide();
    resetScreenTimeout();

    QFile backlightFile(BACKLIGHT_SYS_CLASS);
    if (!backlightFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream commandTextStream(&backlightFile);
    QString command(BACKLIGHT_ON_VALUE);
    commandTextStream << command;
    backlightFile.close();
}

void MainWindow::dimScreen()
{
    QFile backlightFile(BACKLIGHT_SYS_CLASS);
    if (!backlightFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream commandTextStream(&backlightFile);
    QString command(BACKLIGHT_OFF_VALUE);
    commandTextStream << command;
    backlightFile.close();

    backlightOffBtn->show();
    backlightOffBtn->raise();
}

void MainWindow::onSliderPressed()
{
    volumeBarSlider->setProperty("onSliderPressed", true);
    volumeBarSlider->style()->unpolish(volumeBarSlider);
    volumeBarSlider->style()->polish(volumeBarSlider);
}

void MainWindow::onSliderReleased()
{
    volumeBarSlider->setProperty("onSliderPressed", false);
    volumeBarSlider->style()->unpolish(volumeBarSlider);
    volumeBarSlider->style()->polish(volumeBarSlider);
}

void MainWindow::moveUserToSettingsScreen()
{
    settingsScreenButton->setChecked(true);
    onNavBarButtonClicked(static_cast<int>(NavBarButton::SETTINGS));
}

void MainWindow::moveUserToSteamScreen()
{
    steamScreenButton->setChecked(true);
    onNavBarButtonClicked(static_cast<int>(NavBarButton::STEAM));
}

void MainWindow::moveToUserScreen()
{
    powerButton->setChecked(true);
    onNavBarButtonClicked(static_cast<int>(NavBarButton::POWER));
}


