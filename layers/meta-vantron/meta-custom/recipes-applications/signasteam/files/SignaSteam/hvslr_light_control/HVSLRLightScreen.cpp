#include "HVSLRLightScreen.h"
#include "ui_HVSLRLightScreen-landscape.h"
#include "ui_HVSLRLightScreen-portrait.h"

#include "HVSLRLightScreenModel.h"
#include "clickablelabel.h"
#include "colorselector.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSettings>
#include <QStyle>
#include <QTimer>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QPalette>
#include <QtMath>


#include <QDebug>

HVSLRLightScreen::HVSLRLightScreen(QWidget *parent, HVSLRLightScreenModel *HVSLRlightScreenModel) :
    QWidget(parent)
{
    this->HVSLRlightScreenModel = HVSLRlightScreenModel;

    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        HVSLRuiLandscape = new Ui::HVSLRLightScreenLandscape;
        HVSLRuiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        HVSLRuiPortrait = new Ui::HVSLRLightScreenPortrait;
        HVSLRuiPortrait->setupUi(this);
    }

    lightIntensityBarSlider = findChild<QSlider*>("lightIntensityBarSlider");
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        lightIntensityBarSlider->setStyleSheet(barQSliderStyleSheet::VERTICAL_BAR_QSLIDERS);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        lightIntensityBarSlider->setStyleSheet(barQSliderStyleSheet::HORIZONTAL_BAR_QSLIDERS);
    }

    connect(lightIntensityBarSlider, &QSlider::valueChanged, HVSLRlightScreenModel, &HVSLRLightScreenModel::onLightIntensitySliderChanged);
    connect(HVSLRlightScreenModel, &HVSLRLightScreenModel::lightBrightnessChanged, this, &HVSLRLightScreen::onLightBrightnessChanged);
    connect(lightIntensityBarSlider, &QSlider::sliderPressed, this, &HVSLRLightScreen::onSliderPressed);
    connect(lightIntensityBarSlider, &QSlider::sliderReleased, this, &HVSLRLightScreen::onSliderReleased);
    lightIntensityBarSlider->setValue(HVSLR_LIGHT_DEFAULT_VALUE);

    lightToggleBtn = findChild<QToolButton*>("brightnessToggleBtn");
    connect(lightToggleBtn, &QToolButton::toggled, HVSLRlightScreenModel, &HVSLRLightScreenModel::onLightToggleStateChanged);
    connect(HVSLRlightScreenModel, &HVSLRLightScreenModel::lightToggleChanged, this, &HVSLRLightScreen::onLightToggleChanged);
    connect(HVSLRlightScreenModel, &HVSLRLightScreenModel::brightnessSliderLightToggle, this, &HVSLRLightScreen::onBrightnessSliderLightToggleChanged);

    lightButtonArrowGroup = findChild<QButtonGroup*>("lightButtonArrowGroup");
    lightButtonGroup = findChild<QButtonGroup*>("lightButtonGroup");
    QToolButton *violetLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::VIOLET)]);
    lightButtonGroup->setId(violetLightBtn, static_cast<int>(Light::VIOLET));

    QToolButton *indigoLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::INDIGO)]);
    lightButtonGroup->setId(indigoLightBtn, static_cast<int>(Light::INDIGO));

    QToolButton *blueLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::BLUE)]);
    lightButtonGroup->setId(blueLightBtn, static_cast<int>(Light::BLUE));

    QToolButton *greenLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::GREEN)]);
    lightButtonGroup->setId(greenLightBtn, static_cast<int>(Light::GREEN));

    QToolButton *yellowLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::YELLOW)]);
    lightButtonGroup->setId(yellowLightBtn, static_cast<int>(Light::YELLOW));

    QToolButton *orangeLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::ORANGE)]);
    lightButtonGroup->setId(orangeLightBtn, static_cast<int>(Light::ORANGE));

    QToolButton *redLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::RED)]);
    lightButtonGroup->setId(redLightBtn, static_cast<int>(Light::RED));

    QToolButton *moodLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::MOOD)]);
    lightButtonGroup->setId(moodLightBtn, static_cast<int>(Light::MOOD));

    QToolButton *whiteLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::WHITE)]);
    lightButtonGroup->setId(whiteLightBtn, static_cast<int>(Light::WHITE));

    QToolButton *chromaLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::CHROMA)]);
    lightButtonGroup->setId(chromaLightBtn, static_cast<int>(Light::CHROMA));

    QToolButton *colorLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::COLOR)]);
    lightButtonGroup->setId(colorLightBtn, static_cast<int>(Light::COLOR));

    connect(lightButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), HVSLRlightScreenModel, &HVSLRLightScreenModel::onColorLightBtnClicked);
    connect(HVSLRlightScreenModel, &HVSLRLightScreenModel::lightChanged, this, &HVSLRLightScreen::onColorLightChanged);

    //connect the presets color and white light
    connect(HVSLRlightScreenModel,&HVSLRLightScreenModel::preset1ColorChanged, this, &HVSLRLightScreen::onPreset1ColorChanged);
    connect(HVSLRlightScreenModel,&HVSLRLightScreenModel::preset2ColorChanged, this, &HVSLRLightScreen::onPreset2ColorChanged);
    connect(HVSLRlightScreenModel,&HVSLRLightScreenModel::preset3ColorChanged, this, &HVSLRLightScreen::onPreset3ColorChanged);
    connect(HVSLRlightScreenModel,&HVSLRLightScreenModel::preset1WhiteChanged, this, &HVSLRLightScreen::onPreset1WhiteChanged);
    connect(HVSLRlightScreenModel,&HVSLRLightScreenModel::preset2WhiteChanged, this, &HVSLRLightScreen::onPreset2WhiteChanged);
    connect(HVSLRlightScreenModel,&HVSLRLightScreenModel::preset3WhiteChanged, this, &HVSLRLightScreen::onPreset3WhiteChanged);

    lightDescription = findChild<QLabel*>("lightDescription");
    if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        lightDescription->setVisible(false);
    }

    colorWheelImage.load(":/images/portrait/hvslr_screen/color-spectrum.png");
    whiteWheelImage.load(":/images/common/hvslr_screen/white_spectrum.png");


    QLabel *colorWheel = findChild<ClickableLabel*>("QlableColorWheel");
    connect(colorWheel, SIGNAL(wheelClicked(const QPoint& )), this, SLOT(ColorWheelClicked(const QPoint&)));

    connect(this,&HVSLRLightScreen::colorWheelChanged, HVSLRlightScreenModel, &HVSLRLightScreenModel::onColorWheelChange);

    QLabel *whiteWheel = findChild<ClickableLabel*>("QlableWhiteWheel");
    connect(whiteWheel, SIGNAL(wheelClicked(const QPoint& )), this, SLOT(WhiteWheelClicked(const QPoint&)));

    connect(this,&HVSLRLightScreen::whiteWheelChange, HVSLRlightScreenModel, &HVSLRLightScreenModel::onWhiteWheelChange);

    QToolButton *colorPreset1 = findChild<QToolButton*>("toolButtonColorPreset1");
    connect(colorPreset1,SIGNAL(PresetShortPress(const QString&)), this, SLOT(ColorPresetShortPressButton1(const QString&)));
    connect(colorPreset1,SIGNAL(PresetLongPress(const QString& )),this,SLOT(ColorPresetLongPressButton1(const QString&)));


    QToolButton *colorPreset2 = findChild<QToolButton*>("toolButtonColorPreset2");
    connect(colorPreset2,SIGNAL(PresetShortPress(const QString&)), this, SLOT(ColorPresetShortPressButton2(const QString&)));
    connect(colorPreset2,SIGNAL(PresetLongPress(const QString& )),this,SLOT(ColorPresetLongPressButton2(const QString&)));

    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE)
    {
        QToolButton *colorPreset3 = findChild<QToolButton*>("toolButtonColorPreset3");
        connect(colorPreset3,SIGNAL(PresetShortPress(const QString&)), this, SLOT(ColorPresetShortPressButton3(const QString&)));
        connect(colorPreset3,SIGNAL(PresetLongPress(const QString& )),this,SLOT(ColorPresetLongPressButton3(const QString&)));
    }

    selectorDot = new colorSelector(this);

    selectorDot->resize(100,100);
    selectorDot->hide();


    QLabel *presetLabel = findChild<QLabel*>("presetLabel");
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE)
    {
          presetLabel->setFont(fontStyle::getQuicksandBoldWithSize(50));
    }
    else
    {
          presetLabel->setFont(fontStyle::getQuicksandBoldWithSize(36));
    }

    presetLabel->setText("PRESET");

}

HVSLRLightScreen::~HVSLRLightScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete HVSLRuiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete HVSLRuiPortrait;
    }
}

void HVSLRLightScreen::setUpChroma()
{
    

    HVSLRlightScreenModel->setColorMode(static_cast<int>(Light::CHROMA));
    selectorDot->hide();

    lightDescription->hide();


    QLabel *colorWheel = findChild<ClickableLabel*>("QlableColorWheel");
    colorWheel->hide();

    QLabel *whiteWheel = findChild<ClickableLabel*>("QlableWhiteWheel");
    whiteWheel->hide();

    
    QLabel *bodySpectrum = findChild<QLabel*>("bodySpectrum");
    bodySpectrum->show();

    QToolButton *violetLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::VIOLET)]);
    violetLightBtn->setVisible(true);

    QToolButton *violetLightBtnArrow = findChild<QToolButton*>("violetLightBtnArrow");
    violetLightBtnArrow->setVisible(true);

    QToolButton *indigoLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::INDIGO)]);
    indigoLightBtn->setVisible(true);

    QToolButton *indigoLightBtnArrow = findChild<QToolButton*>("indigoLightBtnArrow");
    indigoLightBtnArrow->setVisible(true);


    QToolButton *blueLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::BLUE)]);
    blueLightBtn->setVisible(true);

    QToolButton *blueLightBtnArrow = findChild<QToolButton*>("blueLightBtnArrow");
    blueLightBtnArrow->setVisible(true);

    QToolButton *greenLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::GREEN)]);
    greenLightBtn->setVisible(true);

    QToolButton *greenLightBtnArrow = findChild<QToolButton*>("greenLightBtnArrow");
    greenLightBtnArrow->setVisible(true);


    QToolButton *yellowLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::YELLOW)]);
    yellowLightBtn->setVisible(true);

    QToolButton *yellowLightBtnArrow = findChild<QToolButton*>("yellowLightBtnArrow");
    yellowLightBtnArrow->setVisible(true);


    QToolButton *orangeLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::ORANGE)]);
    orangeLightBtn->setVisible(true);

    QToolButton *orangeLightBtnArrow = findChild<QToolButton*>("orangeLightBtnArrow");
    orangeLightBtnArrow->setVisible(true);


    QToolButton *redLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::RED)]);
    redLightBtn->setVisible(true);

    QToolButton *redLightBtnArrow = findChild<QToolButton*>("redLightBtnArrow");
    redLightBtnArrow->setVisible(true);

   QToolButton *colorPreset1 = findChild<QToolButton*>("toolButtonColorPreset1");
   //colorPreset1->setStyleSheet("background-color:rgb(0, 0, 168);border-radius: 10px;");
   colorPreset1->hide();

   QToolButton *colorPreset2 = findChild<QToolButton*>("toolButtonColorPreset2");
  // colorPreset2->setStyleSheet("background-color:rgb(0, 100, 0);border-radius: 10px;");
   colorPreset2->hide();

   if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE)
   {
        QToolButton *colorPreset3 = findChild<QToolButton*>("toolButtonColorPreset3");
        colorPreset3->hide();
   }


   QLabel *presetLabel = findChild<QLabel*>("presetLabel");
   presetLabel->hide();

   QToolButton *chromaLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::CHROMA)]);
   QToolButton *colorLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::COLOR)]);
   QToolButton *moodLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::MOOD)]);
   QToolButton *whiteLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::WHITE)]);

   if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
        whiteLightBtn->move(MODE_BUTTON_1_LOCATION);
        colorLightBtn->move(MODE_BUTTON_3_LOCATION);
   }

    colorLightBtn->show();
    moodLightBtn->hide();
    whiteLightBtn->show();

    if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
           chromaLightBtn->hide();
    }else
    {
           chromaLightBtn->show();
    }



}

void HVSLRLightScreen::setUpColor()
{
    //currentLightScreen = "color";

     HVSLRlightScreenModel->setColorMode(static_cast<int>(Light::COLOR));

    fillPresetButton();

    lightDescription->hide();


    QLabel *bodySpectrum = findChild<QLabel*>("bodySpectrum");
    bodySpectrum->hide();

    QLabel *whiteWheel = findChild<ClickableLabel*>("QlableWhiteWheel");
    whiteWheel->hide();


    QLabel *colorWheel = findChild<ClickableLabel*>("QlableColorWheel");
    colorWheel->setPixmap(colorWheelImage);
    colorWheel->show();

    QToolButton *violetLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::VIOLET)]);
    violetLightBtn->setVisible(false);

    QToolButton *violetLightBtnArrow = findChild<QToolButton*>("violetLightBtnArrow");
    violetLightBtnArrow->setVisible(false);

    QToolButton *indigoLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::INDIGO)]);
    indigoLightBtn->setVisible(false);

    QToolButton *indigoLightBtnArrow = findChild<QToolButton*>("indigoLightBtnArrow");
    indigoLightBtnArrow->setVisible(false);


    QToolButton *blueLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::BLUE)]);
    blueLightBtn->setVisible(false);

    QToolButton *blueLightBtnArrow = findChild<QToolButton*>("blueLightBtnArrow");
    blueLightBtnArrow->setVisible(false);

    QToolButton *greenLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::GREEN)]);
    greenLightBtn->setVisible(false);

    QToolButton *greenLightBtnArrow = findChild<QToolButton*>("greenLightBtnArrow");
    greenLightBtnArrow->setVisible(false);


    QToolButton *yellowLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::YELLOW)]);
    yellowLightBtn->setVisible(false);

    QToolButton *yellowLightBtnArrow = findChild<QToolButton*>("yellowLightBtnArrow");
    yellowLightBtnArrow->setVisible(false);


    QToolButton *orangeLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::ORANGE)]);
    orangeLightBtn->setVisible(false);

    QToolButton *orangeLightBtnArrow = findChild<QToolButton*>("orangeLightBtnArrow");
    orangeLightBtnArrow->setVisible(false);


    QToolButton *redLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::RED)]);
    redLightBtn->setVisible(false);

    QToolButton *redLightBtnArrow = findChild<QToolButton*>("redLightBtnArrow");
    redLightBtnArrow->setVisible(false);

   QLabel *presetLabel = findChild<QLabel*>("presetLabel");
   presetLabel->show();



   QToolButton *chromaLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::CHROMA)]);
   QToolButton *colorLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::COLOR)]);
   QToolButton *moodLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::MOOD)]);
   QToolButton *whiteLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::WHITE)]);

   if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
      chromaLightBtn->move(MODE_BUTTON_3_LOCATION);
      moodLightBtn->move(MODE_BUTTON_2_LOCATION);
      whiteLightBtn->move(MODE_BUTTON_1_LOCATION);
   }



    chromaLightBtn->show();
    moodLightBtn->show();
    whiteLightBtn->show();

    if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {

        colorLightBtn->hide();
    }
    else
    {

        colorLightBtn->show();
    }

    colorWheel->raise();
    moodLightBtn->raise();
}

void HVSLRLightScreen::setUpMood()
{
    HVSLRlightScreenModel->setColorMode(static_cast<int>(Light::MOOD));

    selectorDot->hide();

    QLabel *bodySpectrum = findChild<QLabel*>("bodySpectrum");
    bodySpectrum->hide();

    QLabel *whiteWheel = findChild<ClickableLabel*>("QlableWhiteWheel");
    whiteWheel->hide();


    QLabel *colorWheel = findChild<ClickableLabel*>("QlableColorWheel");
    colorWheel->hide();

    QToolButton *violetLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::VIOLET)]);
    violetLightBtn->setVisible(false);

    QToolButton *violetLightBtnArrow = findChild<QToolButton*>("violetLightBtnArrow");
    violetLightBtnArrow->setVisible(false);

    QToolButton *indigoLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::INDIGO)]);
    indigoLightBtn->setVisible(false);

    QToolButton *indigoLightBtnArrow = findChild<QToolButton*>("indigoLightBtnArrow");
    indigoLightBtnArrow->setVisible(false);


    QToolButton *blueLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::BLUE)]);
    blueLightBtn->setVisible(false);

    QToolButton *blueLightBtnArrow = findChild<QToolButton*>("blueLightBtnArrow");
    blueLightBtnArrow->setVisible(false);

    QToolButton *greenLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::GREEN)]);
    greenLightBtn->setVisible(false);

    QToolButton *greenLightBtnArrow = findChild<QToolButton*>("greenLightBtnArrow");
    greenLightBtnArrow->setVisible(false);


    QToolButton *yellowLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::YELLOW)]);
    yellowLightBtn->setVisible(false);

    QToolButton *yellowLightBtnArrow = findChild<QToolButton*>("yellowLightBtnArrow");
    yellowLightBtnArrow->setVisible(false);


    QToolButton *orangeLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::ORANGE)]);
    orangeLightBtn->setVisible(false);

    QToolButton *orangeLightBtnArrow = findChild<QToolButton*>("orangeLightBtnArrow");
    orangeLightBtnArrow->setVisible(false);


    QToolButton *redLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::RED)]);
    redLightBtn->setVisible(false);

    QToolButton *redLightBtnArrow = findChild<QToolButton*>("redLightBtnArrow");
    redLightBtnArrow->setVisible(false);

   QLabel *presetLabel = findChild<QLabel*>("presetLabel");
   presetLabel->hide();

   QToolButton *colorPreset1 = findChild<QToolButton*>("toolButtonColorPreset1");
   //colorPreset1->setStyleSheet("background-color:rgb(0, 0, 168);border-radius: 10px;");
   colorPreset1->hide();

   QToolButton *colorPreset2 = findChild<QToolButton*>("toolButtonColorPreset2");
   //colorPreset2->setStyleSheet("background-color:rgb(0, 100, 0);border-radius: 10px;");
   colorPreset2->hide();

   if(orientationSelected != appPersistence::PORTRAIT_ORIENTATION_VALUE)
   {
        QToolButton *colorPreset3 = findChild<QToolButton*>("toolButtonColorPreset3");
        //colorPreset2->setStyleSheet("background-color:rgb(0, 100, 0);border-radius: 10px;");
        colorPreset3->hide();
    }


   QToolButton *chromaLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::CHROMA)]);
   QToolButton *colorLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::COLOR)]);
   QToolButton *moodLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::MOOD)]);
   QToolButton *whiteLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::WHITE)]);

   if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
        chromaLightBtn->move(MODE_BUTTON_3_LOCATION);
        whiteLightBtn->move(MODE_BUTTON_1_LOCATION);
        colorLightBtn->move(MODE_BUTTON_2_COLOR_LOCATION);
   }

   chromaLightBtn->show();
   colorLightBtn->show();
   whiteLightBtn->show();


   moodLightBtn->hide();


//    if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
//        lightDescription->setPixmap(MOOD_DESC_FILE_PORT);
//    }
//    else
//    {
//        lightDescription->setPixmap(MOOD_DESC_FILE_LAND);
//    }
//    lightDescription->raise();
//    lightDescription->show();

}


void HVSLRLightScreen::setUpWhite()
{

     HVSLRlightScreenModel->setColorMode(static_cast<int>(Light::WHITE));

    fillPresetButton();

     lightDescription->hide();


    QLabel *bodySpectrum = findChild<QLabel*>("bodySpectrum");
    bodySpectrum->setVisible(false);

    QLabel *colorWheel = findChild<ClickableLabel*>("QlableColorWheel");
    colorWheel->hide();

    QLabel *whiteWheel = findChild<ClickableLabel*>("QlableWhiteWheel");
    whiteWheel->setPixmap(whiteWheelImage);
    whiteWheel->show();



    QToolButton *violetLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::VIOLET)]);
    violetLightBtn->setVisible(false);

    QToolButton *violetLightBtnArrow = findChild<QToolButton*>("violetLightBtnArrow");
    violetLightBtnArrow->setVisible(false);

    QToolButton *indigoLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::INDIGO)]);
    indigoLightBtn->setVisible(false);

    QToolButton *indigoLightBtnArrow = findChild<QToolButton*>("indigoLightBtnArrow");
    indigoLightBtnArrow->setVisible(false);


    QToolButton *blueLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::BLUE)]);
    blueLightBtn->setVisible(false);

    QToolButton *blueLightBtnArrow = findChild<QToolButton*>("blueLightBtnArrow");
    blueLightBtnArrow->setVisible(false);

    QToolButton *greenLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::GREEN)]);
    greenLightBtn->setVisible(false);

    QToolButton *greenLightBtnArrow = findChild<QToolButton*>("greenLightBtnArrow");
    greenLightBtnArrow->setVisible(false);


    QToolButton *yellowLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::YELLOW)]);
    yellowLightBtn->setVisible(false);

    QToolButton *yellowLightBtnArrow = findChild<QToolButton*>("yellowLightBtnArrow");
    yellowLightBtnArrow->setVisible(false);


    QToolButton *orangeLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::ORANGE)]);
    orangeLightBtn->setVisible(false);

    QToolButton *orangeLightBtnArrow = findChild<QToolButton*>("orangeLightBtnArrow");
    orangeLightBtnArrow->setVisible(false);


    QToolButton *redLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::RED)]);
    redLightBtn->setVisible(false);

    QToolButton *redLightBtnArrow = findChild<QToolButton*>("redLightBtnArrow");
    redLightBtnArrow->setVisible(false);


   QLabel *presetLabel = findChild<QLabel*>("presetLabel");

   presetLabel->show();


   QToolButton *chromaLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::CHROMA)]);
   QToolButton *colorLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::COLOR)]);
   QToolButton *moodLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::MOOD)]);
   QToolButton *whiteLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::WHITE)]);

   if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
       whiteLightBtn->move(MODE_BUTTON_1_LOCATION);
       colorLightBtn->move(MODE_BUTTON_2_COLOR_LOCATION);
       chromaLightBtn->move(MODE_BUTTON_3_LOCATION);
   }

    moodLightBtn->hide();
    whiteLightBtn->show();
    chromaLightBtn->show();
    colorLightBtn->show();
}

void HVSLRLightScreen::fillPresetButton(void)
{

    QToolButton *PresetButtonOne = nullptr;
    QToolButton *PresetButtonTwo = nullptr;
    QToolButton *PresetButtonThree = nullptr;

    int rgbButtonOne;
    int rgbButtonTwo;
    int rgbButtonThree;

    unsigned int colorMode = HVSLRlightScreenModel->getColorMode();

    if (colorMode != static_cast<unsigned int>(Light::COLOR) && colorMode != static_cast<unsigned int>(Light::WHITE))
    {
        return;
    }

    if( colorMode == static_cast<unsigned int>(Light::COLOR) )
    {
        rgbButtonOne = preset1Color;
        rgbButtonTwo = preset2Color;
        rgbButtonThree = preset3Color;
    }
    else
    {
        rgbButtonOne = preset1White;
        rgbButtonTwo = preset2White;
        rgbButtonThree = preset3White;
    }

    //Set up button 1 collor
    unsigned char red   = (rgbButtonOne>>16)&0xFF;
    unsigned char green = (rgbButtonOne>>8)&0xFF;
    unsigned char blue  = rgbButtonOne&0xFF;

    QString styleSheet = "background-color:rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ");border-radius: 10px;";
    PresetButtonOne = findChild<QToolButton*>("toolButtonColorPreset1");

    if(PresetButtonOne != nullptr)
    {
        PresetButtonOne->setStyleSheet(styleSheet);
        PresetButtonOne->show();
    }

    //Set up button 2 color
    red   = (rgbButtonTwo>>16)&0xFF;
    green = (rgbButtonTwo>>8)&0xFF;
    blue  = rgbButtonTwo&0xFF;

    styleSheet = "background-color:rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ");border-radius: 10px;";
    PresetButtonTwo = findChild<QToolButton*>("toolButtonColorPreset2");

    if(PresetButtonTwo != nullptr)
    {
        PresetButtonTwo->setStyleSheet(styleSheet);
        PresetButtonTwo->show();
    }

    //Set up button 3 color
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE)
    {
        red   = (rgbButtonThree>>16)&0xFF;
        green = (rgbButtonThree>>8)&0xFF;
        blue  = rgbButtonThree&0xFF;

        styleSheet = "background-color:rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ");border-radius: 10px;";
        PresetButtonThree = findChild<QToolButton*>("toolButtonColorPreset3");

        if(PresetButtonThree != nullptr)
        {
            PresetButtonThree->setStyleSheet(styleSheet);
            PresetButtonThree->show();
        }
    }
}


void HVSLRLightScreen::onLightBrightnessChanged(int lightBrightnessValue){
    lightIntensityBarSlider->setValue(lightBrightnessValue);
}

void HVSLRLightScreen::onLightToggleChanged(bool lightToggleState)
{
    lightToggleBtn->setChecked(lightToggleState);
}

void HVSLRLightScreen::onBrightnessSliderLightToggleChanged(bool sliderHasMoved)
{
    if(sliderHasMoved){
        lightToggleBtn->setChecked(true);
    }
}

void HVSLRLightScreen::onColorLightChanged(Light lightButtonClicked)
{
   // lightToggleBtn->setChecked(true);
    QPixmap lightDescriptionImage;
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        switch (lightButtonClicked) {
            case Light::VIOLET:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(VIOLET_DESC_FILE_LAND);
                    lightDescription->setGeometry(VIOLET_DESC_PIX_LAND);
                     fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::INDIGO:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(INDIGO_DESC_FILE_LAND);
                    lightDescription->setGeometry(INDIGO_DESC_PIX_LAND);
                     fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::BLUE:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(BLUE_DESC_FILE_LAND);
                    lightDescription->setGeometry(BLUE_DESC_PIX_LAND);
                     fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::GREEN:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(GREEN_DESC_FILE_LAND);
                    lightDescription->setGeometry(GREEN_DESC_PIX_LAND);
                     fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::YELLOW:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(YELLOW_DESC_FILE_LAND);
                    lightDescription->setGeometry(YELLOW_DESC_PIX_LAND);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::ORANGE:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(ORANGE_DESC_FILE_LAND);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::RED:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(RED_DESC_FILE_LAND);
                    lightDescription->setGeometry(RED_DESC_PIX_LAND);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::MOOD:
                     HVSLRLightScreen::setUpMood();
                     lightDescription->setGeometry(MOOD_DESC_PIX_LAND);
                     lightDescriptionImage = QPixmap(MOOD_DESC_FILE_LAND);
                     fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::WHITE:
                    HVSLRLightScreen::setUpWhite();
                break;
            case Light::CHROMA:
                HVSLRLightScreen::setUpChroma();
                break;
            case Light::COLOR:
                HVSLRLightScreen::setUpColor();
                break;
        }


    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        switch (lightButtonClicked) {
            case Light::VIOLET:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(VIOLET_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::INDIGO:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(INDIGO_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::BLUE:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(BLUE_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::GREEN:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(GREEN_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::YELLOW:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(YELLOW_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::ORANGE:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(ORANGE_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::RED:
                    HVSLRLightScreen::setUpChroma();
                    lightDescriptionImage = QPixmap(RED_DESC_FILE_PORT);
                     fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::MOOD:
                    HVSLRLightScreen::setUpMood();
                    lightDescriptionImage = QPixmap(MOOD_DESC_FILE_PORT);
                    fadeInLightDescription(lightDescriptionImage);
                break;
            case Light::WHITE:
                HVSLRLightScreen::setUpWhite();
                break;
            case Light::CHROMA:
                HVSLRLightScreen::setUpChroma();
                break;
            case Light::COLOR:
                HVSLRLightScreen::setUpColor();
                break;
        }

    }

    int lightBtnClickedAsInt = static_cast<int>(lightButtonClicked);
    if(lightBtnClickedAsInt != MOOD_BTN_ID_INDEX &&
            lightBtnClickedAsInt != WHITE_BTN_ID_INDEX &&
            lightBtnClickedAsInt != CHROMA_BTN_ID_INDEX &&
            lightBtnClickedAsInt != COLOR_BTN_ID_INDEX) {
        QString lightBtnArrowName(LIGHT_BTN_IDS[lightBtnClickedAsInt] + "Arrow");
        QToolButton *clickedLightBtnArrow = findChild<QToolButton*>(lightBtnArrowName);
        clickedLightBtnArrow->setChecked(true);
    }
}

void HVSLRLightScreen::onPreset1ColorChanged(int rgb)
{

    preset1Color = rgb;
    fillPresetButton();

}

void HVSLRLightScreen::onPreset2ColorChanged(int rgb)
{
     preset2Color = rgb;
     fillPresetButton();

}

void HVSLRLightScreen::onPreset3ColorChanged(int rgb)
{
     preset3Color = rgb;
     fillPresetButton();

}

void HVSLRLightScreen::onPreset1WhiteChanged(int rgb)
{
    preset1White = rgb;
    fillPresetButton();
}

void HVSLRLightScreen::onPreset2WhiteChanged(int rgb)
{
    preset2White = rgb;
    fillPresetButton();
}

void HVSLRLightScreen::onPreset3WhiteChanged(int rgb)
{
    preset3White = rgb;
    fillPresetButton();
}


void HVSLRLightScreen::onUserLoggedOut()
{
    if(lightButtonGroup->checkedId() != -1) {
        lightButtonGroup->setExclusive(false);
        lightButtonGroup->checkedButton()->setChecked(false);
        lightButtonGroup->setExclusive(true);
    }

    if(lightButtonArrowGroup->checkedId() != -1) {
        lightButtonArrowGroup->setExclusive(false);
        lightButtonArrowGroup->checkedButton()->setChecked(false);
        lightButtonArrowGroup->setExclusive(true);
    }

    lightDescription->clear();
}

void HVSLRLightScreen::fadeInLightDescription(QPixmap lightDescriptionImage)
{

    if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE)
    {



        QLabel *bodySpectrum = findChild<QLabel*>("bodySpectrum");
        bodySpectrum->hide();

        QToolButton *violetLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::VIOLET)]);
        violetLightBtn->setVisible(false);

        QToolButton *violetLightBtnArrow = findChild<QToolButton*>("violetLightBtnArrow");
        violetLightBtnArrow->setVisible(false);

        QToolButton *indigoLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::INDIGO)]);
        indigoLightBtn->setVisible(false);

        QToolButton *indigoLightBtnArrow = findChild<QToolButton*>("indigoLightBtnArrow");
        indigoLightBtnArrow->setVisible(false);


        QToolButton *blueLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::BLUE)]);
        blueLightBtn->setVisible(false);

        QToolButton *blueLightBtnArrow = findChild<QToolButton*>("blueLightBtnArrow");
        blueLightBtnArrow->setVisible(false);

        QToolButton *greenLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::GREEN)]);
        greenLightBtn->setVisible(false);

        QToolButton *greenLightBtnArrow = findChild<QToolButton*>("greenLightBtnArrow");
        greenLightBtnArrow->setVisible(false);


        QToolButton *yellowLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::YELLOW)]);
        yellowLightBtn->setVisible(false);

        QToolButton *yellowLightBtnArrow = findChild<QToolButton*>("yellowLightBtnArrow");
        yellowLightBtnArrow->setVisible(false);


        QToolButton *orangeLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::ORANGE)]);
        orangeLightBtn->setVisible(false);

        QToolButton *orangeLightBtnArrow = findChild<QToolButton*>("orangeLightBtnArrow");
        orangeLightBtnArrow->setVisible(false);


        QToolButton *redLightBtn = findChild<QToolButton*>(LIGHT_BTN_IDS[static_cast<int>(Light::RED)]);
        redLightBtn->setVisible(false);

        QToolButton *redLightBtnArrow = findChild<QToolButton*>("redLightBtnArrow");
        redLightBtnArrow->setVisible(false);
    }

    lightDescription->setPixmap(lightDescriptionImage);
    lightDescription->raise();
    lightDescription->show();

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    lightDescription->setGraphicsEffect(opacityEffect);
    QPropertyAnimation *fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeInAnimation->setDuration(350);
    fadeInAnimation->setStartValue(0);
    fadeInAnimation->setEndValue(1);
    fadeInAnimation->setEasingCurve(QEasingCurve::InBack);
    fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);

    if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE)
    {
        QTimer::singleShot(5000, this, &HVSLRLightScreen::hideLightDescription);
    }
}

void HVSLRLightScreen::hideLightDescription()
{
    lightDescription->hide();
    HVSLRLightScreen::setUpChroma();
}

void HVSLRLightScreen::onSliderPressed()
{
    lightIntensityBarSlider->setProperty("onSliderPressed", true);
    lightIntensityBarSlider->style()->unpolish(lightIntensityBarSlider);
    lightIntensityBarSlider->style()->polish(lightIntensityBarSlider);
}

void HVSLRLightScreen::onSliderReleased()
{
    lightIntensityBarSlider->setProperty("onSliderPressed", false);
    lightIntensityBarSlider->style()->unpolish(lightIntensityBarSlider);
    lightIntensityBarSlider->style()->polish(lightIntensityBarSlider);
}

void HVSLRLightScreen::ColorWheelClicked(const QPoint& p)
{

    int dx = p.x();
    int dy = p.y();

    QImage img = colorWheelImage.toImage();

    QRgb pixel = img.pixel(dx,dy);

    int x;
    int y;

    x = dx - 125;


    y = dy - 125;


    double h = qSqrt((x*x)+(y*y));




    //qDebug() << "\r\nhypoto = " << h << "x = " << x << "y = " << y << "dx = " << dx << "dy = " << dy << "\r\n";


    if(h > 75 && h<125)
    {



        QLabel *colorWheel = findChild<ClickableLabel*>("QlableColorWheel");
        //qDebug() << "colorWheel x = " << colorWheel->x() << "colorWheel y = " << colorWheel->y();
        selectorDot->move(colorWheel->x() + dx, colorWheel->y() + dy);
        selectorDot->raise();
        selectorDot->show();


        emit colorWheelChanged(pixel);

    }
    //free memory

    img = QImage();
}

void HVSLRLightScreen::WhiteWheelClicked(const QPoint& p)
{
    int dx = p.x();
    int dy = p.y();

    QImage img = whiteWheelImage.toImage();

    QRgb pixel = img.pixel(dx,dy);



    int x;
    int y;

    x = dx - 125;


    y = dy - 125;


    double h = qSqrt((x*x)+(y*y));

    //qDebug() << "hypoto = " << h;

    if(h > 75 && h<125)
    {


        QLabel *whiteWheel = findChild<ClickableLabel*>("QlableWhiteWheel");

        //qDebug() << "whiteWheel x = " << whiteWheel->x() << "whiteWheel y = " << whiteWheel->y();
        selectorDot->move(whiteWheel->x() + dx, whiteWheel->y() + dy);
        selectorDot->raise();
        selectorDot->show();


        emit whiteWheelChange(pixel);

    }
    //free memory

    img = QImage();
}

void HVSLRLightScreen::ColorPresetShortPressButton1(const QString& info)
{
    //qDebug() << "Short press button 1";
    int rgb;

    if(HVSLRlightScreenModel->getColorMode() == static_cast<int>(Light::COLOR))
    {
        rgb = preset1Color;
         emit colorWheelChanged((QRgb)rgb);
    }
    else
    {
        rgb = preset1White;
        emit whiteWheelChange((QRgb)rgb);
    }


    //hide the colorwheel selector dot
    selectorDot->hide();

}

void HVSLRLightScreen::ColorPresetShortPressButton2(const QString& info)
{
    //qDebug() << "Short press button 2";

    int rgb;


    if(HVSLRlightScreenModel->getColorMode() == static_cast<int>(Light::COLOR))
    {
        rgb = preset2Color;
         emit colorWheelChanged((QRgb)rgb);
    }
    else
    {
        rgb = preset2White;
        emit whiteWheelChange((QRgb)rgb);
    }

    //hide the colorwheel selector dot
    selectorDot->hide();


}

void HVSLRLightScreen::ColorPresetLongPressButton1(const QString& info)
{

    if(selectorDot->isVisible())
    {
        unsigned int rgb = HVSLRlightScreenModel->getRGB();

        unsigned char red   = (rgb>>16)&0xFF;
        unsigned char green = (rgb>>8)&0xFF;
        unsigned char blue  = rgb&0xFF;

        QString styleSheet = "background-color:rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ");border-radius: 10px;";

        QToolButton *colorPreset1 = findChild<QToolButton*>("toolButtonColorPreset1");
        colorPreset1->setStyleSheet(styleSheet);
        colorPreset1->show();

        if(HVSLRlightScreenModel->getColorMode() == static_cast<int>(Light::COLOR))
        {
            preset1Color = rgb;
            emit savePresetColor1(rgb);

        }
        else
        {
            preset1White = rgb;
            emit savePresetWhite1(rgb);
        }

    }
}

void HVSLRLightScreen::ColorPresetLongPressButton2(const QString& info)
{
    if(selectorDot->isVisible())
    {
        unsigned int rgb = HVSLRlightScreenModel->getRGB();

        unsigned char red   = (rgb>>16)&0xFF;
        unsigned char green = (rgb>>8)&0xFF;
        unsigned char blue  = rgb&0xFF;

        QString styleSheet = "background-color:rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ");border-radius: 10px;";

        QToolButton *colorPreset2 = findChild<QToolButton*>("toolButtonColorPreset2");
        colorPreset2->setStyleSheet(styleSheet);
        colorPreset2->show();

        if(HVSLRlightScreenModel->getColorMode() == static_cast<int>(Light::COLOR))
        {
            preset2Color = rgb;
            emit savePresetColor2(rgb);

        }
        else
        {
            preset2White = rgb;
            emit savePresetWhite2(rgb);
        }
    }
}

void HVSLRLightScreen::ColorPresetShortPressButton3(const QString& info)
{
    int rgb;


    if(HVSLRlightScreenModel->getColorMode() == static_cast<int>(Light::COLOR))
    {
        rgb = preset3Color;
         emit colorWheelChanged((QRgb)rgb);
    }
    else
    {
        rgb = preset3White;
        emit whiteWheelChange((QRgb)rgb);
    }

    //hide the colorwheel selector dot
    selectorDot->hide();


}


void HVSLRLightScreen::ColorPresetLongPressButton3(const QString& info)
{
    if(selectorDot->isVisible())
    {
        unsigned int rgb = HVSLRlightScreenModel->getRGB();

        unsigned char red   = (rgb>>16)&0xFF;
        unsigned char green = (rgb>>8)&0xFF;
        unsigned char blue  = rgb&0xFF;

        QString styleSheet = "background-color:rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ");border-radius: 10px;";

        QToolButton *colorPreset3 = findChild<QToolButton*>("toolButtonColorPreset3");
        colorPreset3->setStyleSheet(styleSheet);
        colorPreset3->show();

        if(HVSLRlightScreenModel->getColorMode() == static_cast<int>(Light::COLOR))
        {
            preset3Color = rgb;
            emit savePresetColor3(rgb);
        }
        else
        {
            preset3White = rgb;
            emit savePresetWhite3(rgb);
        }
    }
}



void HVSLRLightScreen::paintEvent(QPaintEvent *event)
{

}
