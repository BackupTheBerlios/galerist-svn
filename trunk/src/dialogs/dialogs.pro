INCLUDEPATH += ../../src 
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib 
TEMPLATE = lib 
FORMS += configuration.ui 
HEADERS += configuration.h \
           newgallerywizard.h
SOURCES += configuration.cpp \
           newgallerywizard.cpp
