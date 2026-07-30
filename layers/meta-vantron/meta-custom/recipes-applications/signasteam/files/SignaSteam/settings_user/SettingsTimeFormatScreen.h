#ifndef SETTINGSTIMEFORMATSCREEN_H
#define SETTINGSTIMEFORMATSCREEN_H

#include "application/AppConstants.h"

#include <QToolButton>

namespace Ui {
    class SettingsTimeFormatScreenLandscape;
    class SettingsTimeFormatScreenPortrait;
}

class SettingsScreenModel;

class SettingsTimeFormatScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTimeFormatScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr);
    ~SettingsTimeFormatScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);

public slots:
    void onTimeFormatChanged(QString);

private slots:
    void onBackButtonClicked();

private:
    Ui::SettingsTimeFormatScreenLandscape *uiLandscape;
    Ui::SettingsTimeFormatScreenPortrait *uiPortrait;
    QString orientationSelected;
    QToolButton* twelveHourFormatBtn;
    QToolButton* twentyfourHourFormatBtn;
};

#endif // SETTINGSTIMEFORMATSCREEN_H
