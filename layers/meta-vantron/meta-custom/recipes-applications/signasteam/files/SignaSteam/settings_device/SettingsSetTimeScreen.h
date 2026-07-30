#ifndef SETTINGSSETTIMESCREEN_H
#define SETTINGSSETTIMESCREEN_H

#include "application/AppConstants.h"

#include <QTimeEdit>

namespace Ui {
    class SettingsSetTimeScreenLandscape;
    class SettingsSetTimeScreenPortrait;
}

class SettingsScreenModel;

class SettingsSetTimeScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsSetTimeScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr);
    ~SettingsSetTimeScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

public slots:
    void onTimeFormatChanged(QString);
    void onTimeEditReset();

private slots:
    void onBackButtonClicked();
    void onSetTimeBtnClicked();

private:
    const int TIME_FONT_SIZE_LAND = 103;
    const int TIME_FONT_SIZE_PORT = 73;
    Ui::SettingsSetTimeScreenLandscape *uiLandscape;
    Ui::SettingsSetTimeScreenPortrait *uiPortrait;
    SettingsScreenModel *settingsScreenModel;
    QTimeEdit *timeEdit;
    QString orientationSelected;
    void resetTimeEdit();
};

#endif // SETTINGSSETTIMESCREEN_H
