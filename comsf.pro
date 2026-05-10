QT       += core gui widgets serialport

CONFIG += c++17

TARGET = comsf
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    serialmanager.cpp \
    parser.cpp \
    expressionevaluator.cpp \
    configmanager.cpp \
    widgets/gridcanvas.cpp \
    widgets/dashboardarea.cpp \
    widgets/custombutton.cpp \
    widgets/datadisplay.cpp \
    widgets/logicindicator.cpp \
    dialogs/buttonconfigdialog.cpp \
    dialogs/displayconfigdialog.cpp \
    dialogs/indicatorconfigdialog.cpp

HEADERS += \
    mainwindow.h \
    serialmanager.h \
    parser.h \
    expressionevaluator.h \
    configmanager.h \
    widgets/gridcanvas.h \
    widgets/dashboardarea.h \
    widgets/custombutton.h \
    widgets/datadisplay.h \
    widgets/logicindicator.h \
    dialogs/buttonconfigdialog.h \
    dialogs/displayconfigdialog.h \
    dialogs/indicatorconfigdialog.h

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
