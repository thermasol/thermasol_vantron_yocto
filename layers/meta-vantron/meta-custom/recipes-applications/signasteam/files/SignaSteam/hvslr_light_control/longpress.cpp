#include "longpress.h"
#include <QPainter>
#include <QDateTime>
#include <QDebug>

LongPress::LongPress(QWidget* parent, Qt::WindowFlags f) : QToolButton(parent) {

}

LongPress::~LongPress() {}


void LongPress::mousePressEvent(QMouseEvent *event)
{
    //remeber time mouse was pressed
    mLastPresstime = QDateTime::currentMSecsSinceEpoch();
}

void LongPress::mouseReleaseEvent(QMouseEvent *event)
{
    //calculate how long button has been pressed and released
    const quint64 pressTime = QDateTime::currentMSecsSinceEpoch() - mLastPresstime;


    if( pressTime > LONG_PRESS_THRESHOLD)
    {

         emit PresetLongPress("");
    }
    else
    {
        emit PresetShortPress("");
    }
}
