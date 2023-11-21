cmake_minimum_required(VERSION 3.10)

project(wfc_prefixdb)

include(cmake/ci.cmake)
include(cmake/rocksdb.cmake)

ogenc_warning(-Wzero-as-null-pointer-constant "" OFF)


wci_submodule(NAME wfc SUPERMODULE)
get_rocksdb()

add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  include_directories(package)
  add_subdirectory(tests)
endif()
