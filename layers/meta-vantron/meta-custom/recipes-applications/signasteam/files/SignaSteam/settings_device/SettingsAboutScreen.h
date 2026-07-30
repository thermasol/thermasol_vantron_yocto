#ifndef SETTINGSABOUTSCREEN_H
#define SETTINGSABOUTSCREEN_H

#include "application/AppConstants.h"

#include <QWidget>
#include <QPushButton>

namespace Ui {
    class SettingsAboutScreenLandscape;
    class SettingsAboutScreenPortrait;
}

class SettingsAboutScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsAboutScreen(QWidget *parent = nullptr);
    ~SettingsAboutScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);
    void showModeBtnClicked(bool);

private slots:
    void onBackButtonClicked();


private:
    const int ABOUT_SETTINGS_LABEL_FONT_SIZE = 28;
    const int ABOUT_SETTINGS_SUBLABEL_FONT_SIZE = 20;
    QString orientationSelected;
    QPushButton* testBtn;
    Ui::SettingsAboutScreenLandscape *uiLandscape;
    Ui::SettingsAboutScreenPortrait *uiPortrait;
};

#endif // SETTINGSABOUTSCREEN_H
