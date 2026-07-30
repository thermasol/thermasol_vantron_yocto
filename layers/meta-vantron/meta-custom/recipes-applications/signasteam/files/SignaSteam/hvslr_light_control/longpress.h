#ifndef LONGPRESS_H
#define LONGPRESS_H

#include <QToolButton>
#include <QtWidgets>
#include <Qt>

class LongPress : public QToolButton {
    Q_OBJECT

private:
    quint64 mLastPresstime=0;
    static const quint64 LONG_PRESS_THRESHOLD = 1500;

public:
    explicit LongPress(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~LongPress();

signals:
    void PresetLongPress(const QString&);
    void PresetShortPress(const QString&);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // LONGPRESS_H
