TEMPLATE = subdirs
CONFIG += ordered \
warn_on

contains(UPDATE, ts) {
system(cd src/translations)
system(lupdate ../ -ts english.ts)
system(cd ../..)
} else {

unix|win32-g++{
    MAKE = make
}


CONFIG(release, debug|release):print(Goya will build in RELEASE mode.)
CONFIG(debug, debug|release):print(Goya will build in DEBUG mode.)

unix{
print(***************************************)
    isEmpty(PREFIX): PREFIX = /usr

    print(Goya will use $${PREFIX} as its prefix)
    print("To change it, define PREFIX like qmake PREFIX=/path")
    system(cd src && qmake PREFIX=$${PREFIX})
}
print(***************************************)
print("Goya is now configured. To start compiling, run $${MAKE}")
SUBDIRS += src/core/network \
  src/core/jobs \
  src/core \
  src/widgets \
  src/widgets/wizard \
  src/widgets/photowidgets \
  src/dialogs \
  src/translations \
  src

}