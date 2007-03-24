TEMPLATE = subdirs
CONFIG += ordered \
warn_on
SUBDIRS += src/core/network \
  src/core/jobs \
  src/core \
  src/widgets \
  src/widgets/wizard \
  src/dialogs \
  src/translations \
  src

win32-msvc*: MAKE = nmake

unix | win32-g++: {
MAKE = make
isEmpty(PREFIX): PREFIX = /usr

message(Goya will use $${PREFIX} as its prefix)
message("To change it, define PREFIX like qmake PREFIX=/path")
system(cd src && qmake PREFIX=$${PREFIX})
}
message(***************************************)
message("Goya is now configured. To start compiling, run $${MAKE}")
