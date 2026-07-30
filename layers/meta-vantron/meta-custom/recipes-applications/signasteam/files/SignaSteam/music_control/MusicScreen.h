#ifndef MUSICSCREEN_H
#define MUSICSCREEN_H

#include "application/AppConstants.h"

#include <QButtonGroup>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>
#include <QSettings>
#include "EqLineQSlider.h"

namespace Ui {
    class MusicScreenLandscape;
    class MusicScreenPortrait;
}

class MusicScreenModel;

class MusicScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MusicScreen(QWidget *parent = nullptr, MusicScreenModel *musicScreenModel = nullptr);
    ~MusicScreen();

public slots:
    void onMusicInputStateChanged(QString, QString);
    void onTrebleChanged(int);
    void onMidChanged(int);
    void onBassChanged(int);
    void onBtEquipmentDetected(bool);
private slots:
    void onAuxChanged(bool);
    void onBtChanged(bool);

private:
    const int MUSIC_INPUT_LABEL_FONT = 22;
    const int EQUALIZER_DEFAULT_VALUE = 8;
    const QString QSLIDER_STYLESHEET =
        "QSlider::handle:horizontal {"
            "image: url(:/images/common/music_screen/Control_Dot_Normal_30x30.png);"
            "margin: -15px 0;"
        "}"
        "QSlider::groove:horizontal {"
            "background-color: #999999;"
            "height: 3px;"
        "}"
        "QSlider::sub-page:horizontal {"
            "background-color: #FF5A00;"
        "}";
    Ui::MusicScreenLandscape *uiLandscape;
    Ui::MusicScreenPortrait *uiPortrait;
    MusicScreenModel *musicScreenModel;
    QString loggedInUser;
    EqLineQSlider *trebleControlSlider;
    EqLineQSlider *midControlSlider;
    EqLineQSlider *bassControlSlider;
    QString orientationSelected;
    QToolButton *pauseBtn;
    QToolButton *playBtn;
    QToolButton *trackSkipBackBtn;
    QToolButton *trackSkipForwardBtn;
    QSettings settings;
    QToolButton* auxInputBtn;
    QToolButton *bluetoothInputBtn;
    QLabel *deviceInputLabel;
    QLabel *musicInputLabel;
    bool isBtAvailable;
    bool isAuxAvailable;
};

#endif // MUSICSCREEN_H
