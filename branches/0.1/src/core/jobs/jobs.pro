QT += sql 
INCLUDEPATH += ../../../src 
DESTDIR = ./ 
CONFIG += warn_on \
          qt \
          thread \
          staticlib 
TEMPLATE = lib 
HEADERS += abstractjob.h \
           copyjob.h \
           readjob.h 
SOURCES += abstractjob.cpp \
           copyjob.cpp \
           readjob.cpp 
