cmake_minimum_required(VERSION 3.1)

project(wfc_prefixdb)

include(cmake/ci.cmake)
include(cmake/rocksdb.cmake)

wci_getlib(NAME wfc ${WCI_SUPERMODULE})
wci_getlib(NAME rocksdb SUPERMODULE)

set_target_properties(
  rocksdb 
  PROPERTIES   
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
    POSITION_INDEPENDENT_CODE OFF
)

set_target_properties(
  rocksdb-shared
  PROPERTIES   
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib"
    POSITION_INDEPENDENT_CODE ON
)

add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  include_directories(package)
  add_subdirectory(tests)
endif()
