#include "SteamScreen.h"
#include "ui_SteamScreen-landscape.h"
#include "ui_SteamScreen-portrait.h"

#include "SteamScreenModel.h"

#include <QMessageBox>
#include <QSettings>

#include <QDebug>

SteamScreen::SteamScreen(QWidget *parent, SteamScreenModel *steamScreenModel) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::SteamScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::SteamScreenPortrait;
        uiPortrait->setupUi(this);
    }

    steamTemp = findChild<QLabel*>("steamTemp");
    steamTemp->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));
    connect(steamScreenModel, &SteamScreenModel::steamTempChanged, this, &SteamScreen::onSteamTempChanged);

    QToolButton* decreaseSteamTempBtn = findChild<QToolButton*>("decreaseSteamTempBtn");
    connect(decreaseSteamTempBtn, &QToolButton::clicked, steamScreenModel, &SteamScreenModel::onDecreaseTempBtnClicked);

    QToolButton* increaseSteamTempBtn = findChild<QToolButton*>("increaseSteamTempBtn");
    connect(increaseSteamTempBtn, &QToolButton::clicked, steamScreenModel, &SteamScreenModel::onIncreaseTempBtnClicked);

    steamTimeMinutes = findChild<QLabel*>("steamTimeMinutes");
    steamTimeMinutes->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));
    connect(steamScreenModel, &SteamScreenModel::steamTimeChanged, this, &SteamScreen::onSteamTimeChanged);

    steamTimeSeconds = findChild<QLabel*>("steamTimeSeconds");
    steamTimeSeconds->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));

    QToolButton* decreaseSteamTimeBtn = findChild<QToolButton*>("decreaseSteamTimeBtn");
    connect(decreaseSteamTimeBtn, &QToolButton::clicked, steamScreenModel, &SteamScreenModel::onDecreaseTimeBtnClicked);

    QToolButton* increaseSteamTimeBtn = findChild<QToolButton*>("increaseSteamTimeBtn");
    connect(increaseSteamTimeBtn, &QToolButton::clicked, steamScreenModel, &SteamScreenModel::onIncreaseTimeBtnClicked);

    slideToStartArrowsLeft = findChild<SwipeEnabledQLabel*>("slideToStartArrowsLeft");
    slideToStartArrowsLeft->setValidSwipeDistance(SWIPE_TO_TURN_ON_DISTANCE);
    connect(slideToStartArrowsLeft, &SwipeEnabledQLabel::swiped, this, &SteamScreen::onSlideToStartSwiped);
    connect(slideToStartArrowsLeft, &SwipeEnabledQLabel::swiped, steamScreenModel, &SteamScreenModel::onSlideToStartSwiped);

    slideToStartLabel = findChild<QLabel*>("slideToStartLabel");
    slideToStartArrowsRight = findChild<QLabel*>("slideToStartArrowsRight");

    stopSessionButton = findChild<QToolButton*>("stopSessionButton");
    stopSessionButton->hide();
    connect(stopSessionButton, &QToolButton::clicked, this, &SteamScreen::onStopSessionClicked);
    connect(stopSessionButton, &QToolButton::clicked, steamScreenModel, &SteamScreenModel::onStopSessionClicked);
    connect(steamScreenModel, &SteamScreenModel::steamSessionTimedOut, this, &SteamScreen::onSteamSessionTimedOut);

    pfNeededBtn = findChild<QToolButton*>("pfNeededBtn");
    pfNeededBtn->hide();
    connect(pfNeededBtn, &QToolButton::clicked, this, &SteamScreen::pfBtnClicked);
    pfInProgressBtn = findChild<QToolButton*>("pfInProgressBtn");
    pfInProgressBtn->hide();
    connect(pfInProgressBtn, &QToolButton::clicked, this, &SteamScreen::pfBtnClicked);
}

SteamScreen::~SteamScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void SteamScreen::onSteamTempChanged(double steamTempValue)
{
    int tempAsInt = static_cast<int>(steamTempValue);
    steamTemp->setText(QString::number(tempAsInt) + DEGREE_MARK);
}

void SteamScreen::onSteamTimeChanged(int steamTimeValue)
{
    QString minutes = QString("%1").arg(steamTimeValue / 60, 2, 10, QChar('0'));
    QString seconds = QString("%1").arg(steamTimeValue % 60, 2, 10, QChar('0'));

    steamTimeMinutes->setText(minutes);
    steamTimeSeconds->setText(seconds);
}

void SteamScreen::onSteamSessionTimedOut()
{
    showSteamTurnOnControls();
}

void SteamScreen::onPsbStateChanged(QPair<PsbState, PsbErrorState> psbState)
{
    switch (psbState.first) {
        //case PsbState::STEAM_ON:
            //onSlideToStartSwiped();
            //break;
        //case PsbState::STAND_BY:
            //onStopSessionClicked();
            //break;
        case PsbState::PF_NEEDED:
            slideToStartArrowsLeft->hide();
            slideToStartLabel->hide();
            slideToStartArrowsRight->hide();
            if(!stopSessionButton->isHidden()) {
                stopSessionButton->hide();
                stopSessionButton->click();
            }
            pfInProgressBtn->hide();
            pfNeededBtn->show();

            if(!pfNeededDialogShown) {
                showDialog(PF_NEEDED_MESSAGE, PF_DIALOG_FONT_SIZE);
                pfNeededDialogShown = true;
            }
            break;
        case PsbState::PF_IN_PROGRESS:
            slideToStartArrowsLeft->hide();
            slideToStartLabel->hide();
            slideToStartArrowsRight->hide();
            if(!stopSessionButton->isHidden()) {
                stopSessionButton->hide();
                stopSessionButton->click();
            }
            pfInProgressBtn->show();
            pfNeededBtn->hide();
            break;
        case PsbState::PF_FINISHED:
            pfInProgressBtn->hide();
            pfNeededBtn->hide();
            showSteamTurnOnControls();
            pfNeededDialogShown = false;
            break;
        case PsbState::ERROR:
            handleErrorState(psbState.second);
            break;
    }
}

void SteamScreen::onSlideToStartSwiped()
{
    slideToStartArrowsLeft->hide();
    slideToStartLabel->hide();
    slideToStartArrowsRight->hide();
    stopSessionButton->show();
}

void SteamScreen::onStopSessionClicked()
{
    showSteamTurnOnControls();
}

void SteamScreen::showSteamTurnOnControls()
{
    stopSessionButton->hide();
    slideToStartArrowsLeft->show();
    slideToStartLabel->show();
    slideToStartArrowsRight->show();
}

void SteamScreen::handleErrorState(PsbErrorState errorState)
{
    QString errorMessage;

    switch (errorState) {
        case PsbErrorState::OVERTEMP:
            errorMessage = OVERTEMP_ERROR_MESSSAGE;
            break;
        case PsbErrorState::LOW_WATER:
            errorMessage = LOW_WATER_ERROR_MESSSAGE;
            break;
        case PsbErrorState::VOLTAGE_LOW:
            errorMessage = VOLTAGE_LOW_ERROR_MESSSAGE;
            break;
    }

    if(!errorMessage.isEmpty()) {
        if(!errorDialogShown) {
            showDialog(errorMessage + CALL_SERVICE_MESSAGE, ERROR_DIALOG_FONT_SIZE);
            errorDialogShown = true;
        }
    }
}

void SteamScreen::showDialog(QString message, int fontSize)
{
    QToolButton *okayBtn = new QToolButton();
    okayBtn->setStyleSheet(dialogStyle::BUTTON_STYLESHEET);

    QMessageBox dialog;
    dialog.addButton(okayBtn, QMessageBox::ButtonRole::AcceptRole);
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        dialog.setStyleSheet(DIALOG_STYLESHEET_LANDSCAPE);
        dialog.setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        dialog.setStyleSheet(DIALOG_STYLESHEET_PORTRAIT);
        dialog.setFont(fontStyle::getQuicksandBoldWithSize(fontSize));
    }
    dialog.setText(message);
    dialog.exec();
}

