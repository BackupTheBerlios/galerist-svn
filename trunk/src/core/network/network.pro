QT += network
INCLUDEPATH += ../../../src
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib
TEMPLATE = lib
SOURCES += updater.cpp
HEADERS += updater.h

win32 {
DEFINES += WANT_UPDATER
}