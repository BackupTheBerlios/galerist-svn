QT += network
INCLUDEPATH += ../../../src
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib
TEMPLATE = lib
SOURCES += uniupdatemanager.cpp
HEADERS += uniupdatemanager.h

win32 {
    DEFINES += WANT_UPDATER
}