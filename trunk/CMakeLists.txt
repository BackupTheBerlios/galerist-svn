project(Goya)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)
find_package(ImageMagick REQUIRED)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)

add_subdirectory(src)