include(${QT_USE_FILE})

set(wizard_SRCS
  welcomepage.cpp
  selectionpage.cpp
  summarypage.cpp
  copypage.cpp
  finishpage.cpp
)

SET(wizard_UIS
  selectionpage.ui
  summarypage.ui
  copypage.ui
)

SET(wizard_MOC_HDRS
  welcomepage.h
  selectionpage.h
  summarypage.h
  copypage.h
  finishpage.h
)

QT4_WRAP_CPP(wizard_MOC_SRCS ${wizard_MOC_HDRS})

QT4_WRAP_UI(wizard_UIS_H ${wizard_UIS})

include_directories(../../ ${CMAKE_CURRENT_BINARY_DIR})

add_library(wizard STATIC ${wizard_SRCS} ${wizard_UIS_H} ${wizard_MOC_SRCS})

TARGET_LINK_LIBRARIES(wizard widgets ${QT_LIBRARIES})

add_dependencies(wizard widgets)
