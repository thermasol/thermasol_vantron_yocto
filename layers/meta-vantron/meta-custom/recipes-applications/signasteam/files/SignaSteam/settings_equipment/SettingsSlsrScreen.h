#ifndef SETTINGSSLSRSCREEN_H
#define SETTINGSSLSRSCREEN_H

#include "application/AppConstants.h"

#include <QLabel>
#include <QToolButton>
#include <QWidget>
#include <QSlider>

namespace Ui {
    class SettingsSlsrScreenLandscape;
    class SettingsSlsrScreenPortrait;
}

class BluetoothCanInterface;
class SettingsScreenModel;
class EquipmentCanInterface;

class SettingsSlsrScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsSlsrScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr, BluetoothCanInterface *bluetoothCanInterface = nullptr, EquipmentCanInterface *equipmentCanInterface = nullptr);
    ~SettingsSlsrScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

public slots:
    void onVoiceVolumeSliderVoiceToggleChanged(bool);
    void onVoiceVolumeChanged(int);

private slots:
    void onBackButtonClicked();
    void onVoiceToggleChanged(bool);

private:
    const int DEFAULT_VOICE_VALUE = 15;
    const int FONT_SIZE = 24;
    Ui::SettingsSlsrScreenLandscape *uiLandscape;
    Ui::SettingsSlsrScreenPortrait *uiPortrait;
    SettingsScreenModel *settingsScreenModel;
    BluetoothCanInterface *bluetoothCanInterface;
    QString orientationSelected;
    QSlider *voiceVolumeBarSlider;
    QToolButton *voiceOnOffBtn;
    QLabel *voiceSectionLabel;
    QLabel *voiceVolumeIcon;
};

#endif // SETTINGSSLSRSCREEN_H
