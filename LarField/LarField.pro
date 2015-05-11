#-------------------------------------------------
#
# Project created by QtCreator 2015-01-07T14:17:28
#
#-------------------------------------------------

QT       += core gui
QT += xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LarField
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    customscene.cpp \
    desenharcampo.cpp \
    fieldedit.cpp \
    fileselector.cpp

HEADERS  += mainwindow.h \
    customscene.h \
    desenharcampo.h \
    fieldedit.h \
    fileselector.h

FORMS    += mainwindow.ui \
    fieldedit.ui

RESOURCES += \
    rs.qrc
