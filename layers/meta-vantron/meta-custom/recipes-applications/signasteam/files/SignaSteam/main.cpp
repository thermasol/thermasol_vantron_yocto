#include "application/MainWindow.h"

#include "testing/TestBuild.h"

#include <QApplication>
#include <QFontDatabase>
#include <QSettings>

#include <QDebug>
#include "development/DevFunctions.h"

int main(int argc, char *argv[]){
    // Set platform plugin for embedded target (eglfs instead of xcb)
    #ifdef ARM
    qputenv("QT_QPA_PLATFORM", "eglfs");
    #endif

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ThermaSol");
    QCoreApplication::setApplicationName("SignaSteam");
    QSettings::setPath(QSettings::Format::NativeFormat, QSettings::Scope::UserScope, "/opt/");

    QSettings settings;

    int fontFamilyId = QFontDatabase::addApplicationFont(":/fonts/Quicksand-Bold.ttf");
    settings.setValue(fontStyle::QUICKSAND_BOLD_FAMILY_ID, fontFamilyId);

    if(!settings.value(appPersistence::ORIENTATION_SELECTED_KEY).isValid()) {
        settings.setValue(appPersistence::ORIENTATION_SELECTED_KEY, appPersistence::LANDSCAPE_ORIENTATION_VALUE);
    }

   // //EAT changed this
   // settings.setValue(appPersistence::ORIENTATION_SELECTED_KEY, appPersistence::PORTRAIT_ORIENTATION_VALUE);

    if(true) {
        DevOnlyFunctions::seedRng();
    }

    testBuild::setIsTestBuild(false);

    MainWindow w;
    w.show();

    return a.exec();
}
