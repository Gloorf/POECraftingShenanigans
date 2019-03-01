QT       += core gui widgets

TARGET = POECraftingShenanigans
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

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17 # See https://stackoverflow.com/questions/46610996/cant-use-c17-features-using-g-7-2-in-qtcreator


SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
    src/item.cpp \
    src/mod.cpp \
    src/application.cpp \
    src/baseitemtype.cpp \
    src/fossil.cpp \
    src/modtype.cpp \
    src/ggpk.cpp \
    src/utils.cpp

HEADERS += \
        src/mainwindow.h \
    src/item.h \
    src/mod.h \
    src/application.h \
    src/baseitemtype.h \
    src/fossil.h \
    src/constants.h \
    src/modtype.h \
    src/ggpk.h \
    src/utils.h

FORMS += \
        src/mainwindow.ui

RESOURCES += resources.qrc
