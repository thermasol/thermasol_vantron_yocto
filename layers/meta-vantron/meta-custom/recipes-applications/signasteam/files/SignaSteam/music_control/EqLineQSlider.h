#ifndef EQLINEQSLIDER_H
#define EQLINEQSLIDER_H

#include <QSlider>

class EqLineQSlider : public QSlider
{

public:
    EqLineQSlider(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // EQLINEQSLIDER_H
