FORMS += imageaddprogress.ui
HEADERS += lineedit.h \
           gallerytreeview.h \
           imageaddprogress.h \
           photoview.h \
           photoitem.h \
           photoname.h \
           photodescription.h \
           textedit.h \
           tooltip.h \
           searchbar.h \
           inputzoomdialog.h
SOURCES += lineedit.cpp \
           gallerytreeview.cpp \
           imageaddprogress.cpp \
           photoview.cpp \
           photoitem.cpp \
           photoname.cpp \
           photodescription.cpp \
           textedit.cpp \
           tooltip.cpp \
           searchbar.cpp \
           inputzoomdialog.cpp
QT += opengl
INCLUDEPATH += ../../src
DESTDIR = ./
CONFIG += warn_on \
qt \
thread \
staticlib
TEMPLATE = lib
