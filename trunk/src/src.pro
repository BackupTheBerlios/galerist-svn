FORMS += mainwindow.ui
HEADERS += mainwindow.h \
namespace-def.h
SOURCES += main.cpp \
           mainwindow.cpp
RESOURCES = images.qrc
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
thread \
embed_manifest_exe
TEMPLATE = app
win32-g++{
    LIBS += core/libcore.a \
core/network/libnetwork.a \
core/jobs/libjobs.a \
dialogs/libdialogs.a \
widgets/libwidgets.a \
widgets/wizard/libwizard.a \
translations/libtranslations.a
    TARGETDEPS += core/network/libnetwork.a \
core/jobs/libjobs.a \
core/libcore.a \
widgets/libwidgets.a \
widgets/wizard/libwizard.a \
dialogs/libdialogs.a \
translations/libtranslations.a
}unix{
target.path = $${PREFIX}/bin
INSTALLS += target
    LIBS += core/libcore.a \
core/network/libnetwork.a \
core/jobs/libjobs.a \
dialogs/libdialogs.a \
widgets/libwidgets.a \
widgets/wizard/libwizard.a \
translations/libtranslations.a
    TARGETDEPS += core/network/libnetwork.a \
core/jobs/libjobs.a \
core/libcore.a \
widgets/libwidgets.a \
widgets/wizard/libwizard.a \
dialogs/libdialogs.a \
translations/libtranslations.a
}
win32-msvc*{
    LIBS += core/core.lib \
dialogs/dialogs.lib \
widgets/wizard/wizard.lib \
widgets/widgets.lib \
core/network/network.lib \
core/jobs/jobs.lib
    TARGETDEPS += ../src/dialogs/dialogs.lib \
../src/widgets/wizard/wizard.lib \
../src/widgets/widgets.lib \
../src/core/jobs/jobs.lib \
../src/core/network/network.lib \
../src/core/core.lib
}

win32{
    DEFINES += WANT_UPDATER
}
