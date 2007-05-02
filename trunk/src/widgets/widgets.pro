FORMS += imageaddprogress.ui \
photocontrol-main.ui \
photocontrol-crop.ui
HEADERS += lineedit.h \
           gallerytreeview.h \
           imageaddprogress.h \
           photoview.h \
           textedit.h \
           tooltip.h \
           searchbar.h \
           inputzoomdialog.h \
           photocontrol.h
SOURCES += lineedit.cpp \
           gallerytreeview.cpp \
           imageaddprogress.cpp \
           photoview.cpp \
           textedit.cpp \
           tooltip.cpp \
           searchbar.cpp \
           inputzoomdialog.cpp \
           photocontrol.cpp
QT += opengl
DESTDIR = ./
CONFIG += warn_on \
qt \
thread \
staticlib
TEMPLATE = lib
INCLUDEPATH += ..

