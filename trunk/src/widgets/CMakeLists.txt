add_subdirectory(wizard)

include(${QT_USE_FILE})

set(widgets_SRCS
  lineedit.cpp
  gallerytreeview.cpp
  imageaddprogress.cpp
  tooltip.cpp
  searchbar.cpp
  listview.cpp
  imagedelegate.cpp
)

SET(widgets_UIS
  imageaddprogress.ui
)

SET(widgets_MOC_HDRS
  lineedit.h
  gallerytreeview.h
  imageaddprogress.h
#  tooltip.h
  searchbar.h
  listview.h
  imagedelegate.h
)

QT4_WRAP_CPP(widgets_MOC_SRCS ${widgets_MOC_HDRS})

QT4_WRAP_UI(widgets_UIS_H ${widgets_UIS})

include_directories(../ ${CMAKE_CURRENT_BINARY_DIR})

add_library(widgets STATIC ${widgets_SRCS} ${widgets_UIS_H} ${widgets_MOC_SRCS})

TARGET_LINK_LIBRARIES(widgets wizard ${QT_LIBRARIES})