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
  uniupdate.h
)

QT4_WRAP_CPP(dialogs_MOC_SRCS ${dialogs_MOC_HDRS})

QT4_WRAP_UI(dialogs_UIS_H ${dialogs_UIS})

include_directories(../ ${CMAKE_CURRENT_BINARY_DIR})

add_library(dialogs STATIC ${dialogs_SRCS} ${dialogs_UIS_H} ${dialogs_MOC_SRCS})

TARGET_LINK_LIBRARIES(dialogs widgets ${QT_LIBRARIES})

ADD_DEPENDENCIES(dialogs widgets)