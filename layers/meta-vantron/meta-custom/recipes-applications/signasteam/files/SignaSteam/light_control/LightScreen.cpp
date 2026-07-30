#include "LightScreen.h"
#include "ui_LightScreen-landscape.h"
#include "ui_LightScreen-portrait.h"


#include "LightScreenModel.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSettings>
#include <QStyle>
#include <QTimer>

#include <QDebug>

LightScreen::LightScreen(QWidget *parent, LightScreenModel *lightScreenModel) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::LightScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::LightScreenPortrait;
        uiPortrait->setupUi(this);
    }

    lightScreenModel->setLightHelper(normalLightHelper,steamLightHelper);

    lightIntensityBarSlider = findChild<QSlider*>("lightIntensityBarSlider");
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        lightIntensityBarSlider->setStyleSheet(barQSliderStyleSheet::VERTICAL_BAR_QSLIDERS);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        lightIntensityBarSlider->setStyleSheet(barQSliderStyleSheet::HORIZONTAL_BAR_QSLIDERS);
    }

    connect(lightIntensityBarSlider, &QSlider::valueChanged, lightScreenModel, &LightScreenModel::onLightIntensitySliderChanged);
    connect(lightScreenModel, &LightScreenModel::lightBrightnessChanged, this, &LightScreen::onLightBrightnessChanged);
    connect(lightIntensityBarSlider, &QSlider::sliderPressed, this, &LightScreen::onSliderPressed);
    connect(lightIntensityBarSlider, &QSlider::sliderReleased, this, &LightScreen::onSliderReleased);
    lightIntensityBarSlider->setValue(SLSR_LIGHT_DEFAULT_VALUE);

    lightToggleBtn = findChild<QToolButton*>("brightnessToggleBtn");
    connect(lightToggleBtn, &QToolButton::toggled, lightScreenModel, &LightScreenModel::onLightToggleStateChanged);
    connect(lightScreenModel, &LightScreenModel::lightToggleChanged, this, &LightScreen::onLightToggleChanged);
   // connect(lightScreenModel, &LightScreenModel::brightnessSliderLightToggle, this, &LightScreen::onBrightnessSliderLightToggleChanged);

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

    connect(lightButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), lightScreenModel, &LightScreenModel::onColorLightBtnClicked);
    connect(lightScreenModel, &LightScreenModel::lightChanged, this, &LightScreen::onColorLightChanged);

    connect(lightScreenModel, &LightScreenModel::updateLightHardware, lightScreenModel, &LightScreenModel::updateSLSR);

    lightDescription = findChild<QLabel*>("lightDescription");
    if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        lightDescription->setVisible(false);
    }
}

LightScreen::~LightScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void LightScreen::onLightBrightnessChanged(int lightBrightnessValue){
    lightIntensityBarSlider->setValue(lightBrightnessValue);
}

void LightScreen::onLightToggleChanged(bool lightToggleState)
{
    lightToggleBtn->setChecked(lightToggleState);
}

void LightScreen::onBrightnessSliderLightToggleChanged(bool sliderHasMoved)
{
    if(sliderHasMoved){
//        lightToggleBtn->setChecked(true);
    }
}

void LightScreen::onColorLightChanged(LightMode lightButtonClicked)
{
  //  lightToggleBtn->setChecked(true);
    QPixmap lightDescriptionImage;

    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        switch (lightButtonClicked) {
            case LightMode::VIOLET:
                    lightDescriptionImage = QPixmap(VIOLET_DESC_FILE_LAND);
                    lightDescription->setGeometry(VIOLET_DESC_PIX_LAND);
                break;
            case LightMode::INDIGO:
                    lightDescriptionImage = QPixmap(INDIGO_DESC_FILE_LAND);
                    lightDescription->setGeometry(INDIGO_DESC_PIX_LAND);
                break;
            case LightMode::BLUE:
                    lightDescriptionImage = QPixmap(BLUE_DESC_FILE_LAND);
                    lightDescription->setGeometry(BLUE_DESC_PIX_LAND);
                break;
            case LightMode::GREEN:
                    lightDescriptionImage = QPixmap(GREEN_DESC_FILE_LAND);
                    lightDescription->setGeometry(GREEN_DESC_PIX_LAND);
                break;
            case LightMode::YELLOW:
                    lightDescriptionImage = QPixmap(YELLOW_DESC_FILE_LAND);
                    lightDescription->setGeometry(YELLOW_DESC_PIX_LAND);
                break;
            case LightMode::ORANGE:
                    lightDescriptionImage = QPixmap(ORANGE_DESC_FILE_LAND);
                    lightDescription->setGeometry(ORANGE_DESC_PIX_LAND);
                break;
            case LightMode::RED:
                    lightDescriptionImage = QPixmap(RED_DESC_FILE_LAND);
                    lightDescription->setGeometry(RED_DESC_PIX_LAND);
                break;
            case LightMode::MOOD:
                    lightDescriptionImage = QPixmap(MOOD_DESC_FILE_LAND);
                    lightDescription->setGeometry(MOOD_DESC_PIX_LAND);
                break;
            case LightMode::WHITE:
                    lightDescriptionImage = QPixmap(WHITE_DESC_FILE_LAND);
                    lightDescription->setGeometry(WHITE_DESC_PIX_LAND);
                break;
        }
        lightDescription->setPixmap(lightDescriptionImage);

    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        switch (lightButtonClicked) {
            case LightMode::VIOLET:
                    lightDescriptionImage = QPixmap(VIOLET_DESC_FILE_PORT);
                break;
            case LightMode::INDIGO:
                    lightDescriptionImage = QPixmap(INDIGO_DESC_FILE_PORT);
                break;
            case LightMode::BLUE:
                    lightDescriptionImage = QPixmap(BLUE_DESC_FILE_PORT);
                break;
            case LightMode::GREEN:
                    lightDescriptionImage = QPixmap(GREEN_DESC_FILE_PORT);
                break;
            case LightMode::YELLOW:
                    lightDescriptionImage = QPixmap(YELLOW_DESC_FILE_PORT);
                break;
            case LightMode::ORANGE:
                    lightDescriptionImage = QPixmap(ORANGE_DESC_FILE_PORT);
                break;
            case LightMode::RED:
                    lightDescriptionImage = QPixmap(RED_DESC_FILE_PORT);
                break;
            case LightMode::MOOD:
                    lightDescriptionImage = QPixmap(MOOD_DESC_FILE_PORT);
                break;
            case LightMode::WHITE:
                    lightDescriptionImage = QPixmap(WHITE_DESC_FILE_PORT);
                break;
        }
        fadeInLightDescription(lightDescriptionImage);
    }

    int lightBtnClickedAsInt = static_cast<int>(lightButtonClicked);
    if(lightBtnClickedAsInt != MOOD_BTN_ID_INDEX && lightBtnClickedAsInt != WHITE_BTN_ID_INDEX) {
        QString lightBtnArrowName(LIGHT_BTN_IDS[lightBtnClickedAsInt] + "Arrow");
        QToolButton *clickedLightBtnArrow = findChild<QToolButton*>(lightBtnArrowName);
        clickedLightBtnArrow->setChecked(true);
    }

}

void LightScreen::onUserLoggedOut()
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

void LightScreen::fadeInLightDescription(QPixmap lightDescriptionImage)
{
    lightDescription->setPixmap(lightDescriptionImage);
    lightDescription->show();

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    lightDescription->setGraphicsEffect(opacityEffect);
    QPropertyAnimation *fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeInAnimation->setDuration(350);
    fadeInAnimation->setStartValue(0);
    fadeInAnimation->setEndValue(1);
    fadeInAnimation->setEasingCurve(QEasingCurve::InBack);
    fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);

    QTimer::singleShot(3750, this, &LightScreen::hideLightDescription);
}

void LightScreen::hideLightDescription()
{
    lightDescription->hide();
}

void LightScreen::onSliderPressed()
{
    lightIntensityBarSlider->setProperty("onSliderPressed", true);
    lightIntensityBarSlider->style()->unpolish(lightIntensityBarSlider);
    lightIntensityBarSlider->style()->polish(lightIntensityBarSlider);
}

void LightScreen::onSliderReleased()
{
    lightIntensityBarSlider->setProperty("onSliderPressed", false);
    lightIntensityBarSlider->style()->unpolish(lightIntensityBarSlider);
    lightIntensityBarSlider->style()->polish(lightIntensityBarSlider);
}
