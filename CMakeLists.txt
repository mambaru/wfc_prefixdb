cmake_minimum_required(VERSION 3.1)

project(wfc_prefixdb)

include(cmake/ci.cmake)
include(rocksdb)

wfc_libs()
get_rocksdb()

include_directories(.)

add_subdirectory(package)

if ( BUILD_TESTING )
  add_subdirectory(tests)
endif()
