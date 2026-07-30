#ifndef HVSLRLIGHTSCREEN_H
#define HVSLRLIGHTSCREEN_H

#include "application/AppConstants.h"
#include "clickablelabel.h"
#include "colorselector.h"
#include "longpress.h"

#include <QButtonGroup>
#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include <QPainter>
#include <QColor>

namespace Ui {
    class HVSLRLightScreenLandscape;
    class HVSLRLightScreenPortrait;
}

class HVSLRLightScreenModel;

class HVSLRLightScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HVSLRLightScreen(QWidget *parent = nullptr, HVSLRLightScreenModel *HVSLRlightScreenModel = nullptr);
    ~HVSLRLightScreen();
    virtual void paintEvent(QPaintEvent *event);

signals:
    void lightToggledOff(bool);
    void colorWheelChanged(QRgb);
    void whiteWheelChange(QRgb);
    void savePresetColor1(int);
    void savePresetColor2(int);
    void savePresetColor3(int);
    void savePresetWhite1(int);
    void savePresetWhite2(int);
    void savePresetWhite3(int);

public slots:
    void onColorLightChanged(Light);
    void onLightBrightnessChanged(int);
    void onBrightnessSliderLightToggleChanged(bool);
    void onPreset1ColorChanged(int);
    void onPreset2ColorChanged(int);
    void onPreset3ColorChanged(int);
    void onPreset1WhiteChanged(int);
    void onPreset2WhiteChanged(int);
    void onPreset3WhiteChanged(int);
    void onUserLoggedOut();


private slots:
    void hideLightDescription();
    void onSliderPressed();
    void onSliderReleased();
    void onLightToggleChanged (bool);
    void ColorWheelClicked(const QPoint&);
    void WhiteWheelClicked(const QPoint&);
    void ColorPresetShortPressButton1(const QString&);
    void ColorPresetShortPressButton2(const QString&);
    void ColorPresetShortPressButton3(const QString&);
    void ColorPresetLongPressButton1(const QString&);
    void ColorPresetLongPressButton2(const QString&);
    void ColorPresetLongPressButton3(const QString&);


private:
    void setUpChroma();
    void setUpColor();
    void setUpWhite();
    void setUpMood();
    void fillPresetButton();


    const int MOOD_BTN_ID_INDEX = 7;
    const int WHITE_BTN_ID_INDEX = 8;
    const int CHROMA_BTN_ID_INDEX = 9;
    const int COLOR_BTN_ID_INDEX = 10;
    const QPoint MODE_BUTTON_1_LOCATION = QPoint(7,506);
    const QPoint MODE_BUTTON_2_LOCATION = QPoint(142,510);
    const QPoint MODE_BUTTON_2_COLOR_LOCATION = QPoint(120,506);
    const QPoint MODE_BUTTON_3_LOCATION = QPoint(248,506);
    const int HVSLR_LIGHT_DEFAULT_VALUE = 100;
    const QRect VIOLET_DESC_PIX_LAND = QRect(351, 9, 369, 311);
    const QRect INDIGO_DESC_PIX_LAND = QRect(352, 43, 364, 216);
    const QRect BLUE_DESC_PIX_LAND = QRect(340, 0, 390, 362);
    const QRect GREEN_DESC_PIX_LAND = QRect(369, 0, 353, 336);
    const QRect YELLOW_DESC_PIX_LAND = QRect(351, 0, 369, 336);
    const QRect ORANGE_DESC_PIX_LAND = QRect(371, 0, 330, 338);
    const QRect RED_DESC_PIX_LAND = QRect(353, 3, 364, 308);
    const QRect MOOD_DESC_PIX_LAND = QRect(375, 5, 328, 278);
    const QRect WHITE_DESC_PIX_LAND = QRect(357, 6, 357, 286);
    const QString VIOLET_DESC_FILE_LAND = ":/images/landscape/light_screen/Violet_Description_Normal_369x311.png";
    const QString INDIGO_DESC_FILE_LAND = ":/images/landscape/light_screen/Indigo_Description_Normal_364x216.png";
    const QString BLUE_DESC_FILE_LAND = ":/images/landscape/light_screen/Blue_Description_Normal_390x362.png";
    const QString GREEN_DESC_FILE_LAND = ":/images/landscape/light_screen/Green_Description_Normal_353x336.png";
    const QString YELLOW_DESC_FILE_LAND = ":/images/landscape/light_screen/Yellow_Description_Normal_369x336.png";
    const QString ORANGE_DESC_FILE_LAND = ":/images/landscape/light_screen/Orange_Description_Normal_330x338.png";
    const QString RED_DESC_FILE_LAND = ":/images/landscape/light_screen/Red_Description_Normal_364x308.png";
    const QString MOOD_DESC_FILE_LAND = ":/images/landscape/light_screen/Mood_Description_Normal_328x278.png";
    const QString WHITE_DESC_FILE_LAND = ":/images/landscape/light_screen/White_Description_Normal_357x286.png";
    const QString VIOLET_DESC_FILE_PORT = ":/images/portrait/light_screen/Violet_Description_Active_Vert_361x603.png";
    const QString INDIGO_DESC_FILE_PORT = ":/images/portrait/light_screen/Indigo_Description_Active_Vert_361x603.png";
    const QString BLUE_DESC_FILE_PORT = ":/images/portrait/light_screen/Blue_Description_Active_Vert_361x603.png";
    const QString GREEN_DESC_FILE_PORT = ":/images/portrait/light_screen/Green_Description_Active_Vert_361x603.png";
    const QString YELLOW_DESC_FILE_PORT = ":/images/portrait/light_screen/Yellow_Description_Active_Vert_361x603.png";
    const QString ORANGE_DESC_FILE_PORT = ":/images/portrait/light_screen/Orange_Description_Active_Vert_361x603.png";
    const QString RED_DESC_FILE_PORT = ":/images/portrait/light_screen/Red_Description_Active_Vert_361x603.png";
    const QString MOOD_DESC_FILE_PORT = ":/images/portrait/hvslr_screen/Mood_Description_Active_Vert_361x603.png";
    const QString WHITE_DESC_FILE_PORT = ":/images/portrait/light_screen/White_Description_Active_Vert_361x603.png";
    const QList<QString> LIGHT_BTN_IDS  = {
        "violetLightBtn",
        "indigoLightBtn",
        "blueLightBtn",
        "greenLightBtn",
        "yellowLightBtn",
        "orangeLightBtn",
        "redLightBtn",
        "moodLightBtn",
        "whiteLightBtn",
        "chromaLightBtn",
        "colorLightBtn"
    };
    Ui::HVSLRLightScreenLandscape *HVSLRuiLandscape;
    Ui::HVSLRLightScreenPortrait *HVSLRuiPortrait;
    HVSLRLightScreenModel *HVSLRlightScreenModel;
    QButtonGroup *lightButtonGroup;
    QToolButton *lightToggleBtn;
    QButtonGroup *lightButtonArrowGroup;
    QLabel *lightDescription;
    QSlider *lightIntensityBarSlider;
    QString orientationSelected;
    void fadeInLightDescription(QPixmap);
    QPixmap colorWheelImage;
    QPixmap whiteWheelImage;
    colorSelector *selectorDot;
    int preset1Color;
    int preset2Color;
    int preset3Color;
    int preset1White;
    int preset2White;
    int preset3White;


};

#endif // HVSLRLIGHTSCREEN_H
