INCLUDEPATH += ../../src 
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib 
TEMPLATE = lib 
FORMS += configuration.ui 
HEADERS += configuration.h \
           gwizard.h  \
           newgallery.h
SOURCES += configuration.cpp \
           gwizard.cpp  \
           newgallery.cpp
