#include "UserScreen.h"
#include "ui_UserScreen-landscape.h"
#include "ui_UserScreen-portrait.h"

extern "C" {
    #include <thermacan.h>
}

#include <QDebug>
#include <QString>

UserScreen::UserScreen(QWidget *parent) :
    QWidget(parent)
{
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::UserScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::UserScreenPortrait;
        uiPortrait->setupUi(this);
    }

    userOneLabel = findChild<QLabel*>("userOneLabel");
    userOneLabel->setFont(fontStyle::getQuicksandBoldWithSize(USER_LABEL_FONT_SIZE));
    userOneLabel->setText(settings.value(appPersistence::USERNAME_KEY + appPersistence::USER_ONE, appPersistence::USERNAME_ONE_DEFAULT_VALUE).toString());

    userTwoLabel = findChild<QLabel*>("userTwoLabel");
    userTwoLabel->setFont(fontStyle::getQuicksandBoldWithSize(USER_LABEL_FONT_SIZE));
    userTwoLabel->setText(settings.value(appPersistence::USERNAME_KEY + appPersistence::USER_TWO, appPersistence::USERNAME_TWO_DEFAULT_VALUE).toString());

    userOneIcon = findChild<QLabel*>("userOneIcon");
    userOneIcon->setPixmap(QPixmap(settings.value(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_ONE, appPersistence::USERNAME_ONE_ICON_DEFAULT_VALUE).toString()));

    userTwoIcon = findChild<QLabel*>("userTwoIcon");
    userTwoIcon->setPixmap(QPixmap(settings.value(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_TWO, appPersistence::USERNAME_TWO_ICON_DEFAULT_VALUE).toString()));

    QButtonGroup *userButtonGroup = findChild<QButtonGroup*>("userButtonGroup");
    QToolButton *userOneBtnClickArea = findChild<QToolButton*>("userOneBtnClickArea");
    userButtonGroup->setId(userOneBtnClickArea, static_cast<int>(User::USER_ONE));
    
    QToolButton *userTwoBtnClickArea = findChild<QToolButton*>("userTwoBtnClickArea");
    userButtonGroup->setId(userTwoBtnClickArea, static_cast<int>(User::USER_TWO));

    ezsTimer = new QTimer(this);
    connect(ezsTimer, &QTimer::timeout, this, &UserScreen::onEzsTimeout);
    ezsTimer->start(1000);

    connect(this, &UserScreen::ezsUserOneClicked, this, &UserScreen::onUserBtnClicked, Qt::UniqueConnection);
    connect(this, &UserScreen::ezsUserTwoClicked, this, &UserScreen::onUserBtnClicked, Qt::UniqueConnection);
    connect(userButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &UserScreen::onUserBtnClicked);


}

UserScreen::~UserScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void UserScreen::onRefreshUserScreen(User userRefreshed)
{
    if(userRefreshed == User::USER_ONE) {
        userOneLabel->setText(settings.value(appPersistence::USERNAME_KEY + appPersistence::USER_ONE, appPersistence::USERNAME_ONE_DEFAULT_VALUE).toString());
        userOneIcon->setPixmap(QPixmap(settings.value(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_ONE, appPersistence::USERNAME_ONE_ICON_DEFAULT_VALUE).toString()));
    } else if(userRefreshed == User::USER_TWO) {
        userTwoIcon->setPixmap(QPixmap(settings.value(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_TWO, appPersistence::USERNAME_TWO_ICON_DEFAULT_VALUE).toString()));
        userTwoLabel->setText(settings.value(appPersistence::USERNAME_KEY + appPersistence::USER_TWO, appPersistence::USERNAME_TWO_DEFAULT_VALUE).toString());
    }
}

void UserScreen::onEzsTimeout(){

    int ezsUser = ezs_get_user();
    if(ezsUser != ezsLastUser){
        switch(ezsUser){
            case EZS_NO_USER:
                if(!ezsAppLaunch){
                    emit ezsExit();
                }
                ezsLastUser = 0;
                break;
            case EZS_USER_1:
                emit ezsUserOneClicked(0);
                ezsLastUser = 1;
                break;
            case EZS_USER_2:
                emit ezsUserTwoClicked(1);
                ezsLastUser = 2;
                break;
        }
    }



}

void UserScreen::onUserBtnClicked(int buttonClickedIdAsInt)
{
    ezsAppLaunch = false;
    int ezsUserChange = buttonClickedIdAsInt + 1;
    psb_user_login(ezsUserChange);
    emit userLoggedIn(static_cast<User>(buttonClickedIdAsInt));
}
