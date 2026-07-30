#ifndef SETTINGSUSERNAMEKEYBOARDSCREEN_H
#define SETTINGSUSERNAMEKEYBOARDSCREEN_H

#include "application/AppConstants.h"

#include <QLabel>
#include <QWidget>

namespace Ui {
    class SettingsUsernameKeyboardScreenLandscape;
    class SettingsUsernameKeyboardScreenPortrait;
}

class SettingsScreenModel;

class SettingsUsernameKeyboardScreen : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsUsernameKeyboardScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr);
    ~SettingsUsernameKeyboardScreen();

signals:
    void usernameEntered(QString);
    void settingsScreenChanged(SettingsScreenAll);

private slots:
    void onUsernameChanged(QString);
    void onKeyBtnClicked(int);
    void onDeleteKeyBtnPressed();
    void onOkayKeyBtnPressed();
    void onCancelKeyBtnPressed();

private:
    const int LARGE_FONT_SIZE = 31;
    const int MEDIUM_FONT_SIZE = 25;
    const QString USERNAME_TOO_LONG_MSG = "Usernames are limited \nto 8 characters";
    const QString USERNAME_TOO_SHORT_MSG = "Usernames must have \nat least 1 character";
    const QString DIALOG_STYLESHEET =
        "QMessageBox {"
            "border: 4px solid #FE5000;"
            "background: black;"
        "}"
        "QLabel {"
            "color: white;"
            "max-width: 350px;"
            "width: 325px;"
            "min-width: 300px;"
            "max-height: 150px;"
            "height: 125px;"
            "min-height: 100px;"
        "}";
    Ui::SettingsUsernameKeyboardScreenLandscape *uiLandscape;
    Ui::SettingsUsernameKeyboardScreenPortrait *uiPortrait;
    QLabel *usernameValueLabel;
    QString orientationSelected;
    void showDialog(QString);
};

#endif // SETTINGSUSERNAMEKEYBOARDSCREEN_H
