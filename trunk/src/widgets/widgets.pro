FORMS += imageaddprogress.ui
HEADERS += lineedit.h \
           gallerytreeview.h \
           imageaddprogress.h \
           photoview.h \
           textedit.h \
           tooltip.h \
           searchbar.h \
           inputzoomdialog.h
SOURCES += lineedit.cpp \
           gallerytreeview.cpp \
           imageaddprogress.cpp \
           photoview.cpp \
           textedit.cpp \
           tooltip.cpp \
           searchbar.cpp \
           inputzoomdialog.cpp
QT += opengl
DESTDIR = ./
CONFIG += warn_on \
qt \
thread \
staticlib
TEMPLATE = lib
INCLUDEPATH += ..

