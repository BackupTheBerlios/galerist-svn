FORMS += imageaddprogress.ui \
         photocontrol.ui
HEADERS += lineedit.h \
           gallerytreeview.h \
           imageaddprogress.h \
           photoview.h \
           photoitem.h \
           photoname.h \
           photodescription.h \
           textedit.h \
           photocontrol.h \
           tooltip.h \
           searchbar.h
SOURCES += lineedit.cpp \
           gallerytreeview.cpp \
           imageaddprogress.cpp \
           photoview.cpp \
           photoitem.cpp \
           photoname.cpp \
           photodescription.cpp \
           textedit.cpp \
           photocontrol.cpp \
           tooltip.cpp \
           searchbar.cpp
QT += opengl
INCLUDEPATH += ../../src
DESTDIR = ./
CONFIG += warn_on \
qt \
thread \
staticlib
TEMPLATE = lib
