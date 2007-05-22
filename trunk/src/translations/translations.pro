INCLUDEPATH += ../../src
CONFIG += warn_on \
          staticlib
TEMPLATE = lib

system("lupdate ../ -ts english.ts")

system("lrelease english.ts -qm english.qm")
