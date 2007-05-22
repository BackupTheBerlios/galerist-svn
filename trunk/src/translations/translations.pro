INCLUDEPATH += ../../src
CONFIG += warn_on \
          staticlib
TEMPLATE = lib

system("lupdate -silent ../ -ts english.ts")

system("lrelease -silent english.ts -qm english.qm")
