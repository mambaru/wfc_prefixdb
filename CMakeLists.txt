cmake_minimum_required(VERSION 3.1)

project(wfc_prefixdb)

include(cmake/ci.cmake)

FUNCTION(third_party_libs libs)
  set(CMAKE_CXX_FLAGS "-fpic")
  getlibs( ${libs} )
ENDFUNCTION()

FUNCTION(get_rocksdb)
  set(WITH_TESTS OFF)
  set(WITH_TOOLS OFF)
  set(WITH_JEMALLOC OFF)
  set(WITH_GFLAGS OFF)
  set(WITH_SNAPPY ON)
  set(WITH_LZ4 ON)
  set(WITH_ZLIB ON)
  set(WITH_BZ2 ON)
  set(WITH_ZSTD ON)
  third_party_libs(facebook-rocksdb)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb/include)
ENDFUNCTION ()

wfc_libs()
get_rocksdb()

if ( STANDALONE_OLD )

  execute_process(
    COMMAND 
      git submodule update --init -- "external/facebook-rocksdb"
    WORKING_DIRECTORY 
      ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE
      EXIT_CODE
    ERROR_QUIET
  )

  getlibs(faslib wjson wlog wflow iow wjrpc wrtstat wfc)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wfc/cmake)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/facebook-rocksdb/cmake/modules)
  
  # https://github.com/facebook/rocksdb/blob/d1b26507bd97791437d870665277b55eb9e7ce95/INSTALL.md
  # zlib - a library for data compression.
  # bzip2 - a library for data compression.
  # lz4 - a library for extremely fast data compression.
  # snappy - a library for fast data compression.
  # zstandard - Fast real-time compression algorithm.

  if ( NOT IGNORE_COMPRESS_LIBS )
    message(STATUS "Use IGNORE_COMPRESS_LIBS=ON for disable 'REQUIRED' for compression library")
    find_package(snappy REQUIRED)
    find_package(lz4 REQUIRED)
    find_package(ZLIB REQUIRED)

    find_package(BZip2 REQUIRED)
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

if ( BUILD_TESTING )
  add_subdirectory(tests)
endif()
