if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  ADD_DEFINITIONS(-DWANT_UPDATER)
  set(UPDATER true)
endif(CMAKE_SYSTEM_NAME STREQUAL Windows)

set(QT_USE_QTOPENGL TRUE)

include(${QT_USE_FILE})

set(dialogs_SRCS
  configuration.cpp
  newgallerywizard.cpp
  uniupdate.cpp
)

SET(dialogs_UIS
  configuration.ui
  uniupdate.ui
)

SET(dialogs_MOC_HDRS
  configuration.h
  newgallerywizard.h
)


if(DEFINED UPDATER)
ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/moc_uniupdate.cxx
                      COMMAND moc -D WANT_UPDATER ${CMAKE_CURRENT_SOURCE_DIR}/uniupdate.h -o ${CMAKE_CURRENT_SOURCE_DIR}/moc_uniupdate.cxx )
SET(dialog_MOC_SRC
  moc_uniupdate.cxx
)
endif(DEFINED UPDATER)

QT4_WRAP_CPP(dialogs_MOC_SRCS ${dialogs_MOC_HDRS} ${dialogs_MOC_UPDATER})

QT4_WRAP_UI(dialogs_UIS_H ${dialogs_UIS})

include_directories(../ ${CMAKE_CURRENT_BINARY_DIR})

add_library(dialogs STATIC ${dialogs_SRCS} ${dialogs_UIS_H} ${dialogs_MOC_SRCS} ${dialog_MOC_SRC})

TARGET_LINK_LIBRARIES(dialogs widgets ${QT_LIBRARIES})

ADD_DEPENDENCIES(dialogs widgets)