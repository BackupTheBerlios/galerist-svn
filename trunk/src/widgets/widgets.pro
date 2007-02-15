FORMS += newcopypage.ui \
         newselectionpage.ui \
         newwelcomepage.ui \
         newfinishpage.ui \
         imageaddprogress.ui \
         photocontrol.ui  \
         newimageselectpage.ui
HEADERS += newwelcomepage.h \
           newselectionpage.h \
           newcopypage.h \
           newfinishpage.h \
           lineedit.h \
           gallerytreeview.h \
           imageaddprogress.h \
           photoview.h \
           photoitem.h \
           photoname.h \
           photodescription.h \
           textedit.h \
           photocontrol.h \
           tooltip.h  \
           wizardpage.h \
           newimageselectpage.h
SOURCES += newwelcomepage.cpp \
           newselectionpage.cpp \
           newcopypage.cpp \
           newfinishpage.cpp \
           lineedit.cpp \
           gallerytreeview.cpp \
           imageaddprogress.cpp \
           photoview.cpp \
           photoitem.cpp \
           photoname.cpp \
           photodescription.cpp \
           textedit.cpp \
           photocontrol.cpp \
           tooltip.cpp  \
           wizardpage.cpp \
           newimageselectpage.cpp
QT += opengl
INCLUDEPATH += ../../src
DESTDIR = ./
CONFIG += warn_on \
qt \
thread \
staticlib
TEMPLATE = lib
