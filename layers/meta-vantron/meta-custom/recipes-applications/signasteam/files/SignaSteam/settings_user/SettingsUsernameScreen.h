#ifndef SETTINGSUSERNAMESCREEN_H
#define SETTINGSUSERNAMESCREEN_H

#include "application/AppConstants.h"

#include <QLabel>
#include <QToolButton>

namespace Ui {
    class SettingsUsernameScreenLandscape;
    class SettingsUsernameScreenPortrait;
}

class SettingsScreenModel;

class SettingsUsernameScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsUsernameScreen(QWidget *parent = nullptr, SettingsScreenModel *settingsScreenModel = nullptr);
    ~SettingsUsernameScreen();

signals:
    void settingsScreenChanged(SettingsScreenAll);
    void usernameChanged(QString);
    void textEditChanged(QString);

public slots:
    void onUserIconChanged(QString);
    void onUsernameChanged(QString);

private slots:
    void onBackButtonClicked();
    void onSaveBtnClicked();
    void onUsernameClicked();

private:
    const int USERNAME_FONT_SIZE = 36;
    Ui::SettingsUsernameScreenLandscape *uiLandscape;
    Ui::SettingsUsernameScreenPortrait *uiPortrait;
    SettingsScreenModel *settingsScreenModel;
    QLabel *userIcon;
    QLabel *usernameText;
    QToolButton *usernameTextClickArea;
    QString orientationSelected;
    QString username;

};

#endif // SETTINGSUSERNAMESCREEN_H
