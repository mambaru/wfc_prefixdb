cmake_minimum_required(VERSION 3.1)

project(wfc_prefixdb)

include(cmake/ci.cmake)
include(rocksdb)

wfc_libs()
get_rocksdb()

include_directories(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -Wno-undef")
add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  include_directories(package)
  add_subdirectory(tests)
endif()
