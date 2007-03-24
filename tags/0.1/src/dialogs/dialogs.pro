INCLUDEPATH += ../../src 
DESTDIR = ./
CONFIG += warn_on \
          qt \
          thread \
          staticlib 
TEMPLATE = lib 
FORMS += configuration.ui 
HEADERS += newwizard.h \
           wizard.h \
           configuration.h 
SOURCES += newwizard.cpp \
           wizard.cpp \
           configuration.cpp 
