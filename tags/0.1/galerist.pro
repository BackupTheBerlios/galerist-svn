SUBDIRS += src/core/network \
           src/core/jobs \
           src/core \
           src/widgets \
           src/dialogs \
           src/translations \
           src 
TEMPLATE = subdirs
CONFIG += ordered \
warn_on

win32-msvc*: MAKE = nmake

unix | win32-g++: {
MAKE = make
isEmpty(PREFIX): PREFIX = /usr

executable.path = PREFIX/bin
executable.files = bin/goya

message(Goya will use $${PREFIX} as its prefix)
message("To change it, define PREFIX like qmake PREFIX=/path")

INSTALLS += executable

}
message(***************************************)
message("Goya is now configured. To start compiling, run $${MAKE}")
