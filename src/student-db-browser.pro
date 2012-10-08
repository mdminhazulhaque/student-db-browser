#-------------------------------------------------
#
# Project created by QtCreator 2012-06-30T11:31:36
#
#-------------------------------------------------

QT       += core gui xml

TARGET = student-db-browser

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp\
        qcustomplot.cpp

HEADERS  += mainwindow.h\
        qcustomplot.h \
    defines.h

FORMS    += mainwindow.ui

win32: RC_FILE += student-db-browser.rc
