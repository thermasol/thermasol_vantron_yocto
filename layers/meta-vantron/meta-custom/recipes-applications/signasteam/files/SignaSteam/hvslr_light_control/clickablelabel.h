#ifndef CLICKABLE_H
#define CLICKABLE_H

#include <QLabel>
#include <QtWidgets>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

signals:
    void wheelClicked(const QPoint& );

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // CLICKABLE_H
