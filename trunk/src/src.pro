
HEADERS += mainwindow.h \
           namespace-def.h

SOURCES += main.cpp \
           mainwindow.cpp

FORMS += mainwindow.ui

RESOURCES = resources.qrc
RC_FILE = icons.rc
QT += qt \
      sql \
      opengl \
      network

TARGET = goya
DESTDIR = ../bin
CONFIG += warn_on \
          qt \
          opengl \
          thread
TEMPLATE = app
LIBS += core/libcore.a \
        core/jobs/libjobs.a \
        dialogs/libdialogs.a \
        core/network/libnetwork.a \
        widgets/libwidgets.a \
        widgets/photowidgets/libphotowidgets.a \
        widgets/wizard/libwizard.a \
        -lMagick++ \
        -lexif
TARGETDEPS += core/network/libnetwork.a \
              core/jobs/libjobs.a \
              core/libcore.a \
              widgets/libwidgets.a \
              widgets/wizard/libwizard.a \
              dialogs/libdialogs.a \
              widgets/photowidgets/libphotowidgets.a

unix{
target.path = $${PREFIX}/bin
INSTALLS += target
}

win32{
DEFINES += WANT_UPDATER
}

contains (DEFINES, WANT_UPDATER): LIBS += -lXmlRpc
win32: LIBS += -lWand -lMagick -llcms -ljpeg -lpng -lWs2_32 -lgdi32


