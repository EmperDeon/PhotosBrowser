#-------------------------------------------------
#
# Project created by QtCreator 2016-02-18T12:20:22
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotosBrowser
TEMPLATE = app


SOURCES += main.cpp\
resizer.cpp\
imageLoader.cpp\
gifLoader.cpp\
imageProvider.cpp\
pio.cpp

HEADERS  += main.h\
resizer.h\
imageLoader.h\
gifLoader.h\
imageProvider.h\
pio.h

RESOURCES += res.qrc

RC_FILE = res.rc
