include(${QT_USE_FILE})

set(jobs_SRCS
  abstractjob.cpp
  copyjob.cpp
  readjob.cpp
  transformationjob.cpp
  movejob.cpp
)

SET(jobs_MOC_HDRS
  abstractjob.h
  copyjob.h
  readjob.h
  transformationjob.h
  movejob.h
)

QT4_WRAP_CPP(jobs_MOC_SRCS ${jobs_MOC_HDRS})

include_directories(../../ ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

LINK_LIBRARIES(${QT_LIBRARIES})

add_library(jobs STATIC ${jobs_SRCS} ${jobs_MOC_SRCS})

