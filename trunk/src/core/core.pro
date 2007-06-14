QT += sql 
INCLUDEPATH += ../../src 
DESTDIR = ./ 
CONFIG += warn_on \
          qt \
          thread \
          staticlib 
TEMPLATE = lib 
HEADERS += data.h \
           errorhandler.h \
           imageitem.h \
           imagemodel.h \
           metadatamanager.h \
           exifmanager.h \
 jobmanager.h
SOURCES += data.cpp \
           errorhandler.cpp \
           imageitem.cpp \
           imagemodel.cpp \
           metadatamanager.cpp \
           exifmanager.cpp \
 jobmanager.cpp
win32 {
    DEFINES += WANT_UPDATER
}