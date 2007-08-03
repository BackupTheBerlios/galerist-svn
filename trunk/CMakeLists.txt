project(Goya)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)
find_package(ImageMagick REQUIRED)

find_library(exif_LIB exif)

if(NOT exif_LIB)
  message(FATAL_ERROR "Exif library not found!")
endif(NOT exif_LIB)

add_subdirectory(src)