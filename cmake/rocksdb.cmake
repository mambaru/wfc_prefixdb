macro(make_rocksdb)

  message(STATUS "" )
  message(STATUS "************************" )
  message(STATUS "***** ROCKSDB_FOUND=${ROCKSDB_FOUND}" )
  message(STATUS "***** ROCKSDB_LIBRARY=${ROCKSDB_LIBRARY}" )
  message(STATUS "***** ROCKSDB_INCLUDE_DIRS=${ROCKSDB_INCLUDE_DIRS}" )
  message(STATUS "************************" )
  message(STATUS "" )

  add_library(rocksdb SHARED IMPORTED)
  set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${ROCKSDB_LIBRARY} )


  set_property(TARGET rocksdb PROPERTY INTERFACE_INCLUDE_DIRECTORIES
               "${ROCKSDB_INCLUDE_DIRS}" "${ROCKSDB_INCLUDE_DIRS}/..")
  set_property(TARGET rocksdb PROPERTY POSITION_INDEPENDENT_CODE ON)

  target_link_libraries(rocksdb INTERFACE ${SNAPPY_LIBRARIES})
  if ( WITH_COMPRESSION_LIBRARIES )
    set_property(TARGET rocksdb PROPERTY INTERFACE_LINK_LIBRARIES dl zstd lz4 bz2 z)
  endif()
endmacro()

macro(get_rocksdb)

  # Use ROCKSDB_ROOT=/path/to/rocksdb
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
  set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/third_party/")

  find_package(Snappy)
  message(STATUS "Find Snappy=${SNAPPY_FOUND}" )
  if ( NOT SNAPPY_FOUND )
    wci_third_party(NAME snappy PARAMS
      "-DSNAPPY_BUILD_TESTS=OFF -DSNAPPY_BUILD_BENCHMARKS=OFF -DSNAPPY_INSTALL=ON -DCMAKE_CXX_FLAGS=-fPIC" )
    find_package(Snappy REQUIRED)
    message(STATUS "Find Snappy(2)=${SNAPPY_FOUND}" )
  endif()

  find_package(rocksdb)
  message(STATUS "Find RocksDB=${ROCKSDB_FOUND}" )
  if ( NOT ROCKSDB_FOUND )
    wci_third_party(NAME rocksdb BRANCH v7.4.3 PARAMS
      "-DUSE_RTTI=ON -DWITH_SNAPPY=ON -DWITH_TESTS=OFF -DWITH_TOOLS=OFF \
       -DWITH_BENCHMARK_TOOLS=OFF -DWITH_GFLAGS=OFF -DBUILD_SHARED_LIBS=OFF")
    find_package(rocksdb REQUIRED)
    message(STATUS "Find RocksDB(2)=${ROCKSDB_FOUND}" )
  endif()
  make_rocksdb()
endmacro()
