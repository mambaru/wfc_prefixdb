
SET(
  prefixdb_source_list
    prefixdb_package.cpp
    prefixdb/prefixdb_module.cpp
    prefixdb/prefixdb/prefixdb.cpp
    prefixdb/prefixdb/multidb/multidb.cpp
    prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_factory.cpp
    prefixdb/prefixdb/multidb/wrocksdb/wrocksdb.cpp
    prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_restore.cpp
    prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_slave.cpp
    prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_initial.cpp
    prefixdb/prefixdb/multidb/wrocksdb/since_reader.cpp
    prefixdb/prefixdb/multidb/wrocksdb/merge/merge_operator.cpp
    prefixdb/prefixdb/multidb/god.cpp
    prefixdb/prefixdb/multidb/aux/scan_dir.cpp
    prefixdb/prefixdb/multidb/aux/copy_dir.cpp
    prefixdb/prefixdb/prefixdb_multiton.cpp
    prefixdb/service/prefixdb_service_multiton.cpp
    prefixdb/service/prefixdb_cmd.cpp
    prefixdb/gateway/prefixdb_gateway_multiton.cpp
)

add_library(wfc_prefixdb ${prefixdb_source_list})
wci_targets(wfc_prefixdb)

set_source_files_properties(
  prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_factory.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion -Wno-switch-default"
)

set_source_files_properties(
  prefixdb/prefixdb/multidb/wrocksdb/wrocksdb.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion"
)

set_source_files_properties(
  prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_restore.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion"
)

set_source_files_properties(
  prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_slave.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion"
)

set_source_files_properties(
  prefixdb/prefixdb/multidb/wrocksdb/wrocksdb_initial.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion"
)

set_source_files_properties(
  prefixdb/prefixdb/multidb/wrocksdb/since_reader.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion"
)

target_include_directories(wfc_prefixdb PUBLIC  ${wfc_prefixdb_SOURCE_DIR})
target_include_directories(wfc_prefixdb PRIVATE ${wfc_prefixdb_SOURCE_DIR}/package)
target_include_directories(wfc_prefixdb PUBLIC  ${wfc_prefixdb_SOURCE_DIR}/external/rocksdb)
target_include_directories(wfc_prefixdb PUBLIC  ${wfc_prefixdb_SOURCE_DIR}/external/rocksdb/include)

find_library(LIBRT rt)
target_link_libraries(wfc_prefixdb PUBLIC wfc rocksdb ${LIBRT})

add_dependencies(wfc_prefixdb rocksdb)
build_info(wfc_prefixdb prefixdb)
install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})
