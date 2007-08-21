include(${QT_USE_FILE})

set(jobs_SRCS
  abstractjob.cpp
  createjob.cpp
  readjob.cpp
  movejob.cpp
)

SET(jobs_MOC_HDRS
  abstractjob.h
  createjob.h
  readjob.h
  movejob.h
)

QT4_WRAP_CPP(jobs_MOC_SRCS ${jobs_MOC_HDRS})

include_directories(../../ ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

add_library(jobs STATIC ${jobs_SRCS} ${jobs_MOC_SRCS})


TARGET_LINK_LIBRARIES(jobs -lMagick++ ${QT_LIBRARIES})

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  TARGET_LINK_LIBRARIES(jobs -lWand -lMagick -llcms -ljpeg -lpng -lgdi32)
endif(CMAKE_SYSTEM_NAME STREQUAL Windows)
