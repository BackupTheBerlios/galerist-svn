TEMPLATE = lib

CONFIG += warn_on \
staticlib
SOURCES += photodescription.cpp \
photoitem.cpp \
photoname.cpp \
photorect.cpp \
photopixmap.cpp \
photoloading.cpp
HEADERS += photodescription.h \
photoitem.h \
photoname.h \
photorect.h \
photopixmap.h \
photoloading.h
INCLUDEPATH += ../..
DESTDIR = ./
