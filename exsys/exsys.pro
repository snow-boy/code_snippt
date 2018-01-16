#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T17:18:13
#
#-------------------------------------------------

QT       -= gui

TARGET = extensionsystem

include($$PWD/../dll.pri)

SOURCES += \
    pluginloader.cpp \
    servicemanager.cpp

HEADERS  += \
    pluginmacros.h \
    exsys_global.h \
    ilauncher.h \
    pluginloader.h \
    classentry.h \
    servicemanager.h

CONFIG += c++11

DEFINES += EXTENSIONSYSTEM_LIBRARY
