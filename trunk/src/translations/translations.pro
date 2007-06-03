INCLUDEPATH += ../../src
CONFIG += warn_on \
          staticlib
TEMPLATE = lib

system("lrelease english.ts -qm english.qm")
