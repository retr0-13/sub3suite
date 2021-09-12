#-------------------------------------------------
#
# Project created by QtCreator 2020-12-07T00:44:51
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = subsuite
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        src\engines\ContextMenu.cpp \
        src\Logger.cpp \
        src\project\general\GeneralAnalysis.cpp \
        src\project\specific\SpecificAnalysis.cpp \
        src\utils.cpp \
        src\widgets\InputWidget.cpp \
        src\widgets\ResultsWidget.cpp \
        src\engines\CommonScanners.cpp \
        src\project\ProjectDataModel.cpp \
        src\project\Project.cpp \
        src\engines\Base.cpp \
        src\engines\ip\Ip.cpp \
        src\engines\ip\IpScanner.cpp \
        src\engines\active\Active.cpp \
        src\engines\active\ActiveScanner.cpp \
        src\engines\level\Level.cpp \
        src\engines\level\LevelScanner.cpp \
        src\engines\dns\DnsRecords.cpp \
        src\engines\dns\DnsRecordsScanner.cpp \
        src\engines\brute\Brute.cpp \
        src\engines\brute\BruteScanner.cpp \
        src\engines\osint\Osint.cpp \
        src\engines\osint\OsintScanner.cpp \
        src\dialogs\ConfigDialog.cpp \
        src\dialogs\ApiKeysDialog.cpp \
        src\dialogs\WordlistDialog.cpp \
        src\SplashScreen.cpp \
        src\MainWindow.cpp \
        src\main.cpp \

HEADERS += \
    src/Config.h \
        src\engines\AbstractClass.h \
        src\engines\ContextMenu.h \
        src\Logger.h \
        src\project\general\GeneralAnalysis.h \
        src\project\specific\SpecificAnalysis.h \
        src\utils.h \
        src\widgets\InputWidget.h \
        src\widgets\ResultsWidget.h \
        src\engines\CommonScanners.h \
        src\project\ProjectDataModel.h \
        src\project\Project.h \
        src\engines\Base.h \
        src\engines\ip\Ip.h \
        src\engines\ip\IpScanner.h \
        src\engines\active\Active.h \
        src\engines\active\ActiveScanner.h \
        src\engines\level\Level.h \
        src\engines\level\LevelScanner.h \
        src\engines\dns\DnsRecords.h \
        src\engines\dns\DnsRecordsScanner.h \
        src\engines\brute\Brute.h \
        src\engines\brute\BruteScanner.h \
        src\engines\osint\Osint.h \
        src\engines\osint\OsintScanner.h \
        src\dialogs\ApiKeysDialog.h \
        src\dialogs\ConfigDialog.h \
        src\dialogs\WordlistDialog.h \
        src\SplashScreen.h \
        src\MainWindow.h \

FORMS += \
        src\project\general\GeneralAnalysis.ui \
        src\project\specific\SpecificAnalysis.ui \
        src\widgets\InputWidget.ui \
        src\widgets\ResultsWidget.ui \
        src\project\Project.ui \
        src\engines\ip\Ip.ui \
        src\engines\dns\DnsRecords.ui \
        src\engines\level\Level.ui \
        src\engines\brute\Brute.ui \
        src\engines\osint\Osint.ui \
        src\engines\active\Active.ui \
        src\dialogs\ConfigDialog.ui \
        src\dialogs\WordlistDialog.ui \
        src\dialogs\ApiKeysDialog.ui \
        src\MainWindow.ui

# including the python3 headers and libs for compile time...
INCLUDEPATH += -I ../include/python3/include

LIBS += $$OUT_PWD/libs/python38.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

# setting the icon...
RC_ICONS = res/icons/main.ico