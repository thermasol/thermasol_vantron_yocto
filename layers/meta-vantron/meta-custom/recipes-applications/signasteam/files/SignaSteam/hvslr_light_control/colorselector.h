#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include <QWidget>

class colorSelector : public QWidget {
    Q_OBJECT

public:
    explicit colorSelector(QWidget *parent=Q_NULLPTR);
    ~colorSelector();

protected:
    void paintEvent(QPaintEvent *e);

private:
    int x;
    void doPainting();

};

#endif // COLORSELECTOR_H
