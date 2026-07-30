#ifndef DEVFUNCTIONS_H
#define DEVFUNCTIONS_H

#include <QPoint>
#include <QString>

namespace DevOnlyFunctions
{
    void seedRng();
    QString generateRandomString(int);
    int generateRandomNumber(int);
    bool generateTrueOrFalse();
    void randomizePersistenceContents();
}

#endif // DEVFUNCTIONS_H



