if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  ADD_DEFINITIONS(-DWANT_UPDATER)
  set(UPDATER true)
endif(CMAKE_SYSTEM_NAME STREQUAL Windows)

set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})

set(network_SRCS
  uniupdatemanager.cpp
)

if(DEFINED UPDATER)
ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/moc_uniupdatemanager.cxx
                      COMMAND moc -D WANT_UPDATER ${CMAKE_CURRENT_SOURCE_DIR}/uniupdatemanager.h -o ${CMAKE_CURRENT_SOURCE_DIR}/moc_uniupdatemanager.cxx )
SET(network_MOC_SRCS
  moc_uniupdatemanager.cxx
)
endif(DEFINED UPDATER)

include_directories(../../ ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

add_library(network STATIC ${network_SRCS} ${network_MOC_SRCS})

TARGET_LINK_LIBRARIES(network ${QT_LIBRARIES})

if(DEFINED UPDATER)
  TARGET_LINK_LIBRARIES(network -lXmlRpc -lWs2_32)
endif(DEFINED UPDATER)
