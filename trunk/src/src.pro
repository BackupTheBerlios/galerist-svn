FORMS += mainwindow.ui
HEADERS += mainwindow.h
SOURCES += main.cpp \
           mainwindow.cpp
RESOURCES = images.qrc
RC_FILE = icons.rc
QT += qt \
sql \
opengl \
network
LIBS += -L../src \
-L../src/core \
-L../src/core/jobs \
-L../src/core/network \
-L../src/dialogs \
-L../src/widgets \
-lcore \
-ldialogs \
-lwidgets \
-lnetwork \
-ljobs
TARGET = goya
DESTDIR = ../bin
CONFIG += warn_on \
qt \
opengl \
thread \
embed_manifest_exe
TEMPLATE = app
win32-g++ | unix{
  TARGETDEPS += ../src/dialogs/libdialogs.a \
  ../src/widgets/libwidgets.a \
  ../src/core/jobs/libjobs.a \
  ../src/core/network/libnetwork.a \
  ../src/core/libcore.a
}
win32-msvc*{
  TARGETDEPS += ../src/dialogs/dialogs.lib \
  ../src/widgets/widgets.lib \
  ../src/core/jobs/jobs.lib \
  ../src/core/network/network.lib \
  ../src/core/core.lib
}

win32 {
DEFINES += WANT_UPDATER
}
