if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  ADD_DEFINITIONS(-DWANT_UPDATER)
  set(UPDATER true)
endif(CMAKE_SYSTEM_NAME STREQUAL Windows)

add_subdirectory(core)
add_subdirectory(dialogs)
add_subdirectory(widgets)

include(${QT_USE_FILE})

set(goya_SRCS
    main.cpp
    mainwindow.cpp
)

SET(goya_UIS
    mainwindow.ui
)

SET(goya_MOC_HDRS
  mainwindow.h
)

SET( goya_RCS
  resources.qrc
)

QT4_WRAP_CPP(goya_MOC_SRCS ${goya_MOC_HDRS})

QT4_WRAP_UI(goya_UIS_H ${goya_UIS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

QT4_ADD_RESOURCES( goya_RC_SRCS ${goya_RCS} )

IF( MINGW )
  # resource compilation for MinGW
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/goyaicon.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/icons.rc 
                           -o ${CMAKE_CURRENT_BINARY_DIR}/goyaicon.o )
    SET(goya_SRCS ${goya_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/goyaicon.o)
ELSE( MINGW )
    SET(goya_SRCS ${goya_SRCS} icons.rc)
ENDIF( MINGW )

add_executable(../bin/goya ${goya_SRCS} ${goya_UIS_H} ${goya_MOC_SRCS} ${goya_RC_SRCS})

TARGET_LINK_LIBRARIES(../bin/goya dialogs core ${QT_LIBRARIES})

ADD_DEPENDENCIES(../bin/goya core dialogs)