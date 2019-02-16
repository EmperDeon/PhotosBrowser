//
//	Copyright (c) 2017-2019 by Ilya Barykin
//	Released under the MIT License.
//	See the provided LICENSE.TXT file for details.
//

QT     += core gui widgets
CONFIG += c++14

TARGET = PhotosBrowser
TEMPLATE = app


SOURCES += \
 main.cpp \
 GifLoader.cpp \
 ImageLoader.cpp \
 ImageProvider.cpp \
 Pio.cpp \
 Resizer.cpp \
 Window.cpp

HEADERS  += \
 GifLoader.h \
 ImageLoader.h \
 ImageProvider.h \
 Pio.h \
 Resizer.h \
 Window.h

RESOURCES += res/res.qrc

RC_FILE = res/res.rc
