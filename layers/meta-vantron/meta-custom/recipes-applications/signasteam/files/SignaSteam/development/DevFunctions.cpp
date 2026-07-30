#include "DevFunctions.h"

#include "application/AppConstants.h"

#include <QDateTime>
#include <QSettings>

namespace DevOnlyFunctions
{
    void seedRng()
    {
        qsrand(static_cast<unsigned int>(QDateTime::currentSecsSinceEpoch()));
    }

    QString generateRandomString(int length)
    {
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ ");
        const int randomStringLength = length;

        QString randomString;
        for(int i=0; i<randomStringLength; ++i) {
           int index = qrand() % possibleCharacters.length();
           QChar nextChar = possibleCharacters.at(index);
           randomString.append(nextChar);
        }
        return randomString;
    }

    int generateRandomNumber(int max)
    {
        return qrand() % max;
    }

    bool generateTrueOrFalse()
    {
        int randomNumber = 0;
        randomNumber = qrand() % 2;
        bool newRandomState = false;
        if(randomNumber == 0) {
            newRandomState = false;
        } else if(randomNumber == 1) {
            newRandomState = true;
        }

        return newRandomState;
    }

    void randomizePersistenceContents()
    {
        QSettings settings;

        const QList<QString> USER_ICON_PATHS = {
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Golf_Normal_126x126.png",
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Dragonfly_Normal_126x126 copy.png",
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Bird_Normal_126x126",
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Butterfly_Normal_126x126",
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_CowboyHat_Normal_126x126",
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Football_Normal_126x126",
            ":/images/common/settings_screen/username/Settings_Set_Users_Icon_Paw_Normal_126x126"
        };

        const QList<QString> TIME_FORMATS = {
            appPersistence::TIME_FORMAT_TWELVE_HOUR_VALUE,
            appPersistence::TIME_FORMAT_TWENTYFOUR_HOUR_VALUE,
        };

        if(generateTrueOrFalse()) {
            settings.remove(appPersistence::USERNAME_KEY + appPersistence::USER_ONE);
            settings.remove(appPersistence::USERNAME_KEY + appPersistence::USER_TWO);
            settings.remove(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_ONE);
            settings.remove(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_TWO);
            settings.remove(appPersistence::TIME_FORMAT_KEY + appPersistence::USER_ONE);
            settings.remove(appPersistence::TIME_FORMAT_KEY + appPersistence::USER_TWO);
        } else {
            settings.setValue(appPersistence::USERNAME_KEY + appPersistence::USER_ONE, DevOnlyFunctions::generateRandomString(8));
            settings.setValue(appPersistence::USERNAME_KEY + appPersistence::USER_TWO, DevOnlyFunctions::generateRandomString(8));
            settings.setValue(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_ONE, USER_ICON_PATHS[DevOnlyFunctions::generateRandomNumber(USER_ICON_PATHS.size())]);
            settings.setValue(appPersistence::USERNAME_ICON_KEY + appPersistence::USER_TWO, USER_ICON_PATHS[DevOnlyFunctions::generateRandomNumber(USER_ICON_PATHS.size())]);
            settings.setValue(appPersistence::TIME_FORMAT_KEY + appPersistence::USER_ONE, TIME_FORMATS[DevOnlyFunctions::generateRandomNumber(TIME_FORMATS.size())]);
            settings.setValue(appPersistence::TIME_FORMAT_KEY + appPersistence::USER_TWO, TIME_FORMATS[DevOnlyFunctions::generateRandomNumber(TIME_FORMATS.size())]);
        }
    }
}
