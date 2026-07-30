#ifndef SHOWERSCREEN_H
#define SHOWERSCREEN_H

#include "application/AppConstants.h"

#include <QButtonGroup>
#include <QToolButton>
#include <QLabel>

namespace Ui {
    class ShowerScreenLandscape;
    class ShowerScreenPortrait;
}

class ShowerScreenModel;

class ShowerScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ShowerScreen(QWidget *parent = nullptr, ShowerScreenModel *showerScreenModel = nullptr);
    ~ShowerScreen();

signals:
    void waterOutputBtnClicked(WaterOutput);

public slots:
    void onWaterTempChanged(double);
    void onWaterFlowChanged(WaterFlow);
    void onShowerStateChanged(bool);

private slots:
    void onAnyOutputBtnClicked();
    void enableAllBtns();

private:
    const int TEMP_LOW = 45;
    const int LARGE_FONT_SIZE = 108;
    const QRect LOW_FLOW_PIX_LAND = QRect(513, 149, 182, 79);
    const QRect MED_FLOW_PIX_LAND = QRect(506, 149, 195, 79);
    const QRect HIGH_FLOW_PIX_LAND = QRect(502, 149, 207, 98);
    const QRect LOW_FLOW_PIX_PORT = QRect(88, 428, 182, 79);
    const QRect MED_FLOW_PIX_PORT = QRect(75, 427, 195, 79);
    const QRect HIGH_FLOW_PIX_PORT = QRect(77, 427, 207, 93);
    const QString LOW_WATER_VALUE = "Low";
    const QString DEGREE_MARK = "°";
    const QString LOW_FLOW_IMG_PATH = ":images/common/shower_screen/Flow_Display_Low_182x79.png";
    const QString MED_FLOW_IMG_PATH = ":images/common/shower_screen/Flow_Display_Med_195x79.png";
    const QString HIGH_FLOW_IMG_PATH_LAND = ":images/landscape/shower_screen/Flow_Display_High_207x98.png";
    const QString HIGH_FLOW_IMG_PATH_PORT = ":images/portrait/shower_screen/Flow_Display_High_207x93.png";
    Ui::ShowerScreenLandscape *uiLandscape;
    Ui::ShowerScreenPortrait *uiPortrait;
    ShowerScreenModel *showerScreenModel;
    QButtonGroup* waterOutputOptionsBtnGroup;
    QLabel *waterTemp;
    QLabel *waterFlow;
    QString orientationSelected;
    QToolButton *rainHeadBtn;
    QToolButton *showerHeadBtn;
    QToolButton *wandBtn;
};

#endif // SHOWERSCREEN_H
