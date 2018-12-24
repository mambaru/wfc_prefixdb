cmake_minimum_required(VERSION 3.1)

project(wfc_prefixdb)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

if ( STANDALONE )
  getlibs(faslib wjson wlog wflow iow wjrpc wrtstat wfc)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wfc/cmake)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb/cmake/modules)
  message(STATUS "--> ${CMAKE_MODULE_PATH}")
  # https://github.com/facebook/rocksdb/blob/d1b26507bd97791437d870665277b55eb9e7ce95/INSTALL.md
  # zlib - a library for data compression.
  # bzip2 - a library for data compression.
  # lz4 - a library for extremely fast data compression.
  # snappy - a library for fast data compression.
  # zstandard - Fast real-time compression algorithm.

  if ( NOT IGNORE_COMPRESS_LIBS )
    message(STATUS "Use IGNORE_COMPRESS_LIBS=ON for disable 'REQUIRED' for compression library")
    find_package(snappy REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(BZip2 REQUIRED)
    find_package(lz4 REQUIRED)
    find_package(zstd)
  endif()

  if ( BUILD_SHARED_LIBS )
    set(shared_lib "shared_lib")
  else()
    set(shared_lib "static_lib")
  endif()

  message(STATUS "shared_lib=${shared_lib}")
  add_custom_target( 
    facebook-rocksdb
    COMMAND make USE_RTTI=1 ${shared_lib}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb
    VERBATIM
  )
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb/include)
endif()

include_directories(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -Wno-undef -Wno-sign-conversion")
add_subdirectory(package)

if ( BUILD_TESTING AND STANDALONE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})                                                                                                                 
  #add_subdirectory(tests)
endif()
