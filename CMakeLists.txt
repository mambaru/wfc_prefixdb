cmake_minimum_required(VERSION 3.1)

project(wfc_prefixdb)

include(cmake/ci.cmake)
include(cmake/rocksdb.cmake)


wci_getlib(NAME wfc ${WCI_SUPERMODULE})
get_rocksdb()

add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  include_directories(package)
  add_subdirectory(tests)
endif()
