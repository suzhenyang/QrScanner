#-------------------------------------------------
#
# Project created by QtCreator 2016-05-13T01:55:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QrScanner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    AgvCode.cpp \
    AgvImageProcess.cpp \
    AgvSocketClient.cpp \
    AgvCommandProcess.cpp \
    AgvLogs.cpp

HEADERS  += mainwindow.h \
    AgvCode.h \
    AgvImageProcess.h \
    AgvSocketClient.h \
    AgvCommandProcess.h \
    AgvLogs.h

FORMS    += mainwindow.ui



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lMagick++-7.Q16HDRI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lMagick++-7.Q16HDRI
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lMagick++-7.Q16HDRI

INCLUDEPATH += $$PWD/../../../../usr/local/include/ImageMagick-7
DEPENDPATH += $$PWD/../../../../usr/local/include/ImageMagick-7

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lMagickCore-7.Q16HDRI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lMagickCore-7.Q16HDRI
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lMagickCore-7.Q16HDRI

INCLUDEPATH += $$PWD/../../../../usr/local/include/ImageMagick-7/MagickCore
DEPENDPATH += $$PWD/../../../../usr/local/include/ImageMagick-7/MagickCore

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lMagickWand-7.Q16HDRI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lMagickWand-7.Q16HDRI
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lMagickWand-7.Q16HDRI

INCLUDEPATH += $$PWD/../../../../usr/local/include/ImageMagick-7/MagickWand
DEPENDPATH += $$PWD/../../../../usr/local/include/ImageMagick-7/MagickWand

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lopencv_core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lopencv_core
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_core

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lopencv_highgui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lopencv_highgui
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_highgui

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lopencv_imgproc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lopencv_imgproc
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_imgproc

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/release/ -lzbar
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/debug/ -lzbar
else:unix: LIBS += -L$$PWD/../../../../usr/lib/ -lzbar

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -llog4cpp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -llog4cpp
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -llog4cpp

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include
