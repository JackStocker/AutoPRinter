#-------------------------------------------------
#
# Project created by QtCreator 2013-09-22T19:42:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoPRinter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    AutoPRint.qrc

OTHER_FILES += \
    AutoPRinter.rc

win32 {
    RC_FILE = AutoPRinter.rc
}
