#ifndef STEAMSCREEN_H
#define STEAMSCREEN_H

#include "application/AppConstants.h"
#include "SwipeEnabledQLabel.h"

#include <QToolButton>

namespace Ui {
    class SteamScreenLandscape;
    class SteamScreenPortrait;
}

class SteamScreenModel;

class SteamScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SteamScreen(QWidget *parent = nullptr, SteamScreenModel *steamScreenModel = nullptr);
    ~SteamScreen();

signals:
    void pfBtnClicked();

public slots:
    void onSteamTempChanged(double);
    void onSteamTimeChanged(int);
    void onSteamSessionTimedOut();
    void onSlideToStartSwiped();
    void onStopSessionClicked();
    void onPsbStateChanged(QPair<PsbState, PsbErrorState>);

private slots:
    //void onSlideToStartSwiped();
    //void onStopSessionClicked();

private:
    const int LARGE_FONT_SIZE = 108;
    const int ERROR_DIALOG_FONT_SIZE = 20;
    const int PF_DIALOG_FONT_SIZE = 30;
    const int SWIPE_TO_TURN_ON_DISTANCE = 200;
    const QString DEGREE_MARK = "°";
    const QString PF_NEEDED_MESSAGE =
            "Power Flush Needed.\n"
            "Please go to Settings, Equipment, Steam Generator and start a Power Flush.\n"
            "This process must be completed before the generator can be used again.";
    const QString OVERTEMP_ERROR_MESSSAGE =
            "Room Overtemp\n"
            "This error indicates that your steam room exceeded 135°F (57°C) and for your safety the generator automatically shut off.\n"
            "Allow the steam room to cool down.\n"
            "Reset the breaker to clear the error and resume normal use.\n";
    const QString LOW_WATER_ERROR_MESSSAGE =
            "Low water level in the tank\n"
            "This error means that the generator does not sense that there is water in the tank.\n"
            "Check to make sure there is water going to the unit.\n"
            "Check your inline debris screen and clean it if necessary.\n"
            "If the above did not work, initiate a powerflush and let the cycle complete.\n"
            "Reset the breaker to clear the error and resume normal use.\n";
    const QString VOLTAGE_LOW_ERROR_MESSSAGE =
            "Low AC voltage\n"
            "The input voltage to the generator is below the requirement for operation.\n"
            "Reset the breaker to clear the error and resume normal use.\n"
            "If problem continues have an electrician test the voltage at the unit and correct what is needed to get the voltage up to 208‐240V AC.\n";
    const QString CALL_SERVICE_MESSAGE = "If the problem continues, please call service at +1 (800) 776-0711";
    const QString DIALOG_STYLESHEET_LANDSCAPE =
        "QMessageBox {"
            "border: 4px solid #FE5000;"
            "background: black;"
        "}"
        "QLabel {"
            "color: white;"
            "max-width: 600px;"
            "width: 550px;"
            "min-width: 500px;"
            "max-height: 375px;"
            "height: 300px;"
            "min-height: 100px;"
        "}";
    const QString DIALOG_STYLESHEET_PORTRAIT =
        "QMessageBox {"
            "border: 4px solid #FE5000;"
            "background: black;"
        "}"
        "QLabel {"
            "color: white;"
            "max-width: 400px;"
            "width: 375px;"
            "min-width: 350px;"
            "max-height: 425px;"
            "height: 375px;"
            "min-height: 325px;"
        "}";
    bool errorDialogShown = false;
    bool pfNeededDialogShown = false;
    Ui::SteamScreenLandscape *uiLandscape;
    Ui::SteamScreenPortrait *uiPortrait;
    SteamScreenModel *steamScreenModel;
    QLabel *steamTemp;
    QLabel *steamTimeMinutes;
    QLabel *steamTimeSeconds;
    QLabel *slideToStartLabel;
    QLabel *slideToStartArrowsRight;
    QString orientationSelected;
    QToolButton *stopSessionButton;
    QToolButton *pfNeededBtn;
    QToolButton *pfInProgressBtn;
    SwipeEnabledQLabel* slideToStartArrowsLeft;
    void showSteamTurnOnControls();
    void handleErrorState(PsbErrorState);
    void showDialog(QString, int);
};

#endif // STEAMSCREEN_H
