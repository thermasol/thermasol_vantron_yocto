#include "AppConstants.h"

#include <QFontDatabase>
#include <QSettings>

namespace fontStyle {
    QFont getQuicksandBoldWithSize(int pixelSize)
    {
        QSettings settings;

        int fontFamilyId = settings.value(QUICKSAND_BOLD_FAMILY_ID).toInt();
        QString family = QFontDatabase::applicationFontFamilies(fontFamilyId).at(0);
        QFont quicksandBold(family);
        quicksandBold.setPixelSize(pixelSize);

        return quicksandBold;
    }
}
