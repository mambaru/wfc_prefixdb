cmake_minimum_required(VERSION 2.6)

project(wfc_prefixdb)

add_subdirectory(prefixdb)

if ( WFC_BUILD_ALL OR PREFIX_BUILD_ALL )
  add_subdirectory(tests)
endif()
