TEMPLATE = lib

CONFIG += staticlib \
warn_on
HEADERS += welcomepage.h \
selectionpage.h \
summarypage.h \
copypage.h \
finishpage.h
SOURCES += welcomepage.cpp \
selectionpage.cpp \
summarypage.cpp \
copypage.cpp \
finishpage.cpp
FORMS += selectionpage.ui \
summarypage.ui \
copypage.ui
INCLUDEPATH += ../..
DESTDIR = ./
