#include "colorselector.h"
#include <QPainter>


colorSelector::colorSelector(QWidget* parent) : QWidget(parent) {
    x=0;
}

colorSelector::~colorSelector() {}

void colorSelector::paintEvent(QPaintEvent *e)
{
    doPainting();
}

void colorSelector::doPainting()
{
    QPainter cs(this);
    QBrush pintada(Qt::lightGray, Qt::SolidPattern);
    pintada.setColor(Qt::lightGray);
    //cs.setRenderHint(QPainter::Antialiasing, true);
    cs.setPen(QPen(Qt::lightGray,1));
    cs.setBrush(QBrush(pintada));
    cs.drawEllipse(x,x,10,10);

}
