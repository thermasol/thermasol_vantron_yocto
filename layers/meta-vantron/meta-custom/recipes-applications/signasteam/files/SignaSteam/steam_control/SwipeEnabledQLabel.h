#ifndef SWIPEENABLEDQLABEL_H
#define SWIPEENABLEDQLABEL_H

#include <QLabel>

class SwipeEnabledQLabel : public QLabel
{
    Q_OBJECT

public:
    SwipeEnabledQLabel(QWidget *parent = nullptr);
    void setValidSwipeDistance(int);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void swiped();

private:
    const int Y_TOLERANCE = 50;
    int startX = 0;
    int finalX = 0;
    int validSwipeDistance = INT_MAX;
    void resetCoordinates();

};

#endif // SWIPEENABLEDQLABEL_H
