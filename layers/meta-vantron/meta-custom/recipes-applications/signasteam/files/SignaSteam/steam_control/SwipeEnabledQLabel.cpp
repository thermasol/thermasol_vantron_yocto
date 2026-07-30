#include "SwipeEnabledQLabel.h"

#include <QMouseEvent>

#include <QDebug>

SwipeEnabledQLabel::SwipeEnabledQLabel(QWidget *parent)
    : QLabel(parent)
{

}

void SwipeEnabledQLabel::mousePressEvent(QMouseEvent *event)
{
    startX = event->x();
}

void SwipeEnabledQLabel::mouseReleaseEvent(QMouseEvent *event)
{
    finalX = event->x();

    int distanceTraveledByX = finalX - startX;

    if(distanceTraveledByX > validSwipeDistance && event->y() < Y_TOLERANCE) {
        emit swiped();
    }
    resetCoordinates();
}

void SwipeEnabledQLabel::resetCoordinates()
{
    startX = 0;
    finalX = 0;
}

void SwipeEnabledQLabel::setValidSwipeDistance(int validSwipeDistance)
{
    this->validSwipeDistance = validSwipeDistance;
}
