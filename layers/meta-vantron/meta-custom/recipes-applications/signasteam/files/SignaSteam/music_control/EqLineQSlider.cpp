#include "EqLineQSlider.h"

#include <QPainter>

#include <QDebug>

EqLineQSlider::EqLineQSlider(QWidget *parent)
    : QSlider(parent)
{

}

void EqLineQSlider::paintEvent(QPaintEvent *event)
{
    QSlider::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QColor(153,153,153));

    painter.drawLine(63, 7, 63, 25);
    painter.drawLine(128, 0, 128, 50);
    painter.drawLine(193, 7, 193, 25);
}
