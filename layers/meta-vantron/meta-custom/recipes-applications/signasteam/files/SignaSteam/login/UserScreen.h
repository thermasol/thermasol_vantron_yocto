#ifndef USERSCREEN_H
#define USERSCREEN_H

#include "application/AppConstants.h"

#include <QLabel>
#include <QSettings>
#include <QTimer>

namespace Ui {
    class UserScreenLandscape;
    class UserScreenPortrait;
}

class UserScreen : public QWidget
{
    Q_OBJECT

public:
    explicit UserScreen(QWidget *parent = nullptr);
    ~UserScreen();

signals:
    void userLoggedIn(User);
    void ezsUserOneClicked(int);
    void ezsUserTwoClicked(int);
    void ezsExit();
    void isEzsPowerOn(bool);

public slots:
    void onRefreshUserScreen(User);
    void onUserBtnClicked(int);
    void onEzsTimeout();

private:
    const int USER_LABEL_FONT_SIZE = 36;
    Ui::UserScreenLandscape *uiLandscape;
    Ui::UserScreenPortrait *uiPortrait;
    QLabel *userOneIcon;
    QLabel *userTwoIcon;
    QLabel *userOneLabel;
    QLabel *userTwoLabel;
    bool ezsAppLaunch;
    int ezsLastUser;
    bool userLoggedout;
    QTimer *ezsTimer;
    QSettings settings;
    QString orientationSelected;
};

#endif // USERSCREEN_H
