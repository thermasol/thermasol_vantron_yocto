#ifndef SETTINGSTEMPSCALESCREEN_H
#define SETTINGSTEMPSCALESCREEN_H

#include "application/AppConstants.h"

#include <QToolButton>

namespace Ui {
    class SettingsTempScaleScreenLandscape;
    class SettingsTempScaleScreenPortrait;
}

class SettingsScreenModel;

class SettingsTempScaleScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTempScaleScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr);
    ~SettingsTempScaleScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

public slots:
    void onTempScaleChanged(QString);

private slots:
    void onBackButtonClicked();

private:
    Ui::SettingsTempScaleScreenLandscape *uiLandscape;
    Ui::SettingsTempScaleScreenPortrait *uiPortrait;
    QString orientationSelected;
    QToolButton* fahrenheitScaleBtn;
    QToolButton* celciusScaleBtn;
};

#endif // SETTINGSTEMPSCALESCREEN_H
