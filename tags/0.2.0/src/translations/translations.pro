INCLUDEPATH += ../../src
CONFIG += warn_on \
          staticlib
TEMPLATE = lib

system("lrelease -silent english.ts -qm english.qm")
