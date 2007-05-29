QT += network
INCLUDEPATH += ../../../src
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib
TEMPLATE = lib
SOURCES += updater.cpp \
 uniupdatemanager.cpp
HEADERS += updater.h \
 uniupdatemanager.h

win32 {
    DEFINES += WANT_UPDATER
}