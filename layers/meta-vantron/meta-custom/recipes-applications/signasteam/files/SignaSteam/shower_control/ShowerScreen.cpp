#include "ShowerScreen.h"
#include "ui_ShowerScreen-landscape.h"
#include "ui_ShowerScreen-portrait.h"

#include "ShowerScreenModel.h"

#include <QSettings>
#include <QTimer>

#include <QDebug>

ShowerScreen::ShowerScreen(QWidget *parent, ShowerScreenModel *showerScreenModel) :
    QWidget(parent)
{
    QSettings settings;
    orientationSelected = settings.value(appPersistence::ORIENTATION_SELECTED_KEY).toString();
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        uiLandscape = new Ui::ShowerScreenLandscape;
        uiLandscape->setupUi(this);
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        uiPortrait = new Ui::ShowerScreenPortrait;
        uiPortrait->setupUi(this);
    }

    waterTemp = findChild<QLabel*>("waterTemp");
    waterTemp->setFont(fontStyle::getQuicksandBoldWithSize(LARGE_FONT_SIZE));
    connect(showerScreenModel, &ShowerScreenModel::waterTempChanged, this, &ShowerScreen::onWaterTempChanged);

    QToolButton *decreaseWaterTempBtn = findChild<QToolButton*>("decreaseWaterTempBtn");
    connect(decreaseWaterTempBtn, &QToolButton::clicked, showerScreenModel, &ShowerScreenModel::onDecreaseTempBtnClicked);

    QToolButton *increaseWaterTempBtn = findChild<QToolButton*>("increaseWaterTempBtn");
    connect(increaseWaterTempBtn, &QToolButton::clicked, showerScreenModel, &ShowerScreenModel::onIncreaseTempBtnClicked);

    waterOutputOptionsBtnGroup = findChild<QButtonGroup*>("waterOutputOptionsBtnGroup");
    connect(waterOutputOptionsBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), showerScreenModel, &ShowerScreenModel::onWaterOutputBtnClicked);
    connect(showerScreenModel, &ShowerScreenModel::showerStateChanged, this, &ShowerScreen::onShowerStateChanged);

    rainHeadBtn = findChild<QToolButton*>("rainHeadBtn");
    connect(rainHeadBtn, &QToolButton::clicked, this, &ShowerScreen::onAnyOutputBtnClicked);
    waterOutputOptionsBtnGroup->setId(rainHeadBtn, static_cast<int>(WaterOutput::RAINHEAD));

    showerHeadBtn = findChild<QToolButton*>("showerHeadBtn");
    connect(showerHeadBtn, &QToolButton::clicked, this, &ShowerScreen::onAnyOutputBtnClicked);
    waterOutputOptionsBtnGroup->setId(showerHeadBtn, static_cast<int>(WaterOutput::SHOWERHEAD));

    wandBtn = findChild<QToolButton*>("wandBtn");
    connect(wandBtn, &QToolButton::clicked, this, &ShowerScreen::onAnyOutputBtnClicked);
    waterOutputOptionsBtnGroup->setId(wandBtn, static_cast<int>(WaterOutput::WAND));

    waterFlow = findChild<QLabel*>("waterFlow");
    connect(showerScreenModel, &ShowerScreenModel::waterFlowChanged, this, &ShowerScreen::onWaterFlowChanged);

    QToolButton *decreaseWaterFlowBtn = findChild<QToolButton*>("decreaseWaterFlowBtn");
    connect(decreaseWaterFlowBtn, &QToolButton::clicked, showerScreenModel, &ShowerScreenModel::onDecreaseFlowBtnClicked);

    QToolButton *increaseWaterFlowBtn = findChild<QToolButton*>("increaseWaterFlowBtn");
    connect(increaseWaterFlowBtn, &QToolButton::clicked, showerScreenModel, &ShowerScreenModel::onIncreaseFlowBtnClicked);
}

ShowerScreen::~ShowerScreen()
{
    if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
        delete uiLandscape;
    } else if (orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE){
        delete uiPortrait;
    }
}

void ShowerScreen::onWaterTempChanged(double waterTempValue)
{
    int tempAsInt = static_cast<int>(waterTempValue);
    if(tempAsInt == TEMP_LOW) {
        waterTemp->setText(LOW_WATER_VALUE);
    } else {
        waterTemp->setText(QString::number(tempAsInt) + DEGREE_MARK);
    }
}

void ShowerScreen::onWaterFlowChanged(WaterFlow waterFlowValue)
{
    QPixmap waterFlowImage;
    switch(waterFlowValue) {
        case WaterFlow::LOW:
            waterFlowImage = QPixmap(LOW_FLOW_IMG_PATH);
            if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
                waterFlow->setGeometry(LOW_FLOW_PIX_LAND);
            } else if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                waterFlow->setGeometry(LOW_FLOW_PIX_PORT);
            }
            break;
        case WaterFlow::MEDIUM:
            waterFlowImage = QPixmap(MED_FLOW_IMG_PATH);
            if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
                waterFlow->setGeometry(MED_FLOW_PIX_LAND);
            } else if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                waterFlow->setGeometry(MED_FLOW_PIX_PORT);
            }
            break;
        case WaterFlow::HIGH:
            if(orientationSelected == appPersistence::LANDSCAPE_ORIENTATION_VALUE) {
                waterFlowImage = QPixmap(HIGH_FLOW_IMG_PATH_LAND);
                waterFlow->setGeometry(HIGH_FLOW_PIX_LAND);
            } else if(orientationSelected == appPersistence::PORTRAIT_ORIENTATION_VALUE) {
                waterFlowImage = QPixmap(HIGH_FLOW_IMG_PATH_PORT);
                waterFlow->setGeometry(HIGH_FLOW_PIX_PORT);
            }
            break;
    }
    waterFlow->setPixmap(waterFlowImage);
}

void ShowerScreen::onShowerStateChanged(bool showerIsOn)
{
    if(!showerIsOn) {
        waterOutputOptionsBtnGroup->setExclusive(false);
        waterOutputOptionsBtnGroup->checkedButton()->setChecked(false);
        waterOutputOptionsBtnGroup->setExclusive(true);
    }
}

void ShowerScreen::onAnyOutputBtnClicked()
{
    rainHeadBtn->setEnabled(false);
    showerHeadBtn->setEnabled(false);
    wandBtn->setEnabled(false);
    QTimer::singleShot(500, this, &ShowerScreen::enableAllBtns);
}

void ShowerScreen::enableAllBtns()
{
    rainHeadBtn->setEnabled(true);
    showerHeadBtn->setEnabled(true);
    wandBtn->setEnabled(true);
}
