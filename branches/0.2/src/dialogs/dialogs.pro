INCLUDEPATH += ../../src 
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib 
TEMPLATE = lib 
FORMS += configuration.ui  \
 uniupdate.ui
HEADERS += configuration.h \
           newgallerywizard.h \
 uniupdate.h
SOURCES += configuration.cpp \
           newgallerywizard.cpp \
 uniupdate.cpp
win32 {
    DEFINES += WANT_UPDATER

    INCLUDEPATH += ../../src

}
