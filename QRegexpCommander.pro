#-------------------------------------------------
#
# Project created by QtCreator 2014-09-08T09:28:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QRegexpCommander
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    dialogchanges.cpp \
    dialogrule.cpp

HEADERS  += dialog.h \
    dialogchanges.h \
    dialogrule.h

FORMS    += dialog.ui \
    rule.ui \
    dialogchanges.ui
