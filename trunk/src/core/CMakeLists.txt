add_subdirectory(jobs)
add_subdirectory(network)

set(QT_USE_QTSQL TRUE)

include(${QT_USE_FILE})

set(core_SRCS
  data.cpp
  errorhandler.cpp
  imageitem.cpp
  imagemodel.cpp
  metadatamanager.cpp
  exifmanager.cpp
  jobmanager.cpp
)

SET(core_MOC_HDRS
  data.h
  errorhandler.h
  imagemodel.h
  metadatamanager.h
  exifmanager.h
  jobmanager.h
)

QT4_WRAP_CPP(core_MOC_SRCS ${core_MOC_HDRS})

include_directories(../ ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

add_library(core STATIC ${core_SRCS} ${core_MOC_SRCS})

TARGET_LINK_LIBRARIES(core jobs network -lexif ${QT_LIBRARIES})

ADD_DEPENDENCIES(core jobs network)

