macro(get_rocksdb)

  if ( DEFINED ENV{ROCKSDB_ROOT} )
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    find_package(rocksdb)
    message(STATUS "Find RocksDB=${ROCKSDB_FOUND}" )
  endif()

  if ( ROCKSDB_FOUND )
    # USE_RTTI=1 make shared_lib
    message(STATUS "" )
    message(STATUS "************************" )
    message(STATUS "***** RocksDB FOUND=${ROCKSDB_FOUND}" )
    message(STATUS "***** RocksDB_LIBRARY=${ROCKSDB_LIBRARY}" )
    message(STATUS "***** RocksDB_INCLUDE=${ROCKSDB_INCLUDE_DIRS}" )
    message(STATUS "************************" )
    message(STATUS "" )

    add_library(rocksdb SHARED IMPORTED)
    set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${ROCKSDB_LIBRARY} )
    set_property(TARGET rocksdb PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ROCKSDB_INCLUDE_DIRS}" "${ROCKSDB_INCLUDE_DIRS}/..")
    set_property(TARGET rocksdb PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET rocksdb PROPERTY INTERFACE_LINK_LIBRARIES dl zstd snappy lz4 bz2 z)
  else()
    set(USE_RTTI  ON CACHE BOOL "Enabling RTTI")
    set(WITH_TESTS  OFF CACHE BOOL "Build with tests")
    set(WITH_TOOLS  OFF CACHE BOOL "Build with tools")
    set(WITH_GFLAGS OFF CACHE BOOL "Build with GFlags")
    set(WITHOUT_COMPRESSION_LIBRARIES OFF CACHE BOOL "Without compression libraries")
    if ( NOT WITHOUT_COMPRESSION_LIBRARIES )
      set(WITH_SNAPPY ON  CACHE BOOL "Build with SNAPPY")
      set(WITH_LZ4    ON  CACHE BOOL "Build with lz4")
      set(WITH_ZLIB   ON  CACHE BOOL "Build with zlib")
      set(WITH_BZ2    ON  CACHE BOOL "Build with bz2")
      set(WITH_ZSTD   ON  CACHE BOOL "Build with zstd")
    endif()

    wci_remove_options(-Wextra-semi)

    wci_getlib(NAME rocksdb SUPERMODULE)

    set_target_properties(
      rocksdb
      PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        POSITION_INDEPENDENT_CODE ON
    )

    set_target_properties(
      rocksdb-shared
      PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib"
        POSITION_INDEPENDENT_CODE ON
      )

      include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/external/rocksdb/include")
      include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/external/rocksdb")

  endif()

endmacro()
