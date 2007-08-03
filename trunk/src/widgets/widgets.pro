FORMS += imageaddprogress.ui
HEADERS += lineedit.h \
           gallerytreeview.h \
           imageaddprogress.h \
           tooltip.h \
           searchbar.h \
           listview.h
SOURCES += lineedit.cpp \
           gallerytreeview.cpp \
           imageaddprogress.cpp \
           tooltip.cpp \
           searchbar.cpp \
           listview.cpp
QT += opengl
DESTDIR = ./
CONFIG += warn_on \
qt \
thread \
staticlib
TEMPLATE = lib
INCLUDEPATH += ..

