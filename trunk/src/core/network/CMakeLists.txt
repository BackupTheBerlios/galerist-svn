set(QT_USE_NETWORK TRUE)

include(${QT_USE_FILE})

set(network_SRCS
  uniupdatemanager.cpp
)

SET(network_MOC_HDRS
  uniupdatemanager.h
)

QT4_WRAP_CPP(network_MOC_SRCS ${network_MOC_HDRS})

include_directories(../../ ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

add_library(network STATIC ${network_SRCS} ${network_MOC_SRCS})

TARGET_LINK_LIBRARIES(network ${QT_LIBRARIES})

