
include(BuildInfo)

include_directories(.)

SET(
  prefixdb_source_list
  prefixdb/domain/storage/aux/scan_dir.cpp
  prefixdb_package.cpp
  prefixdb/prefixdb_module.cpp
  prefixdb/domain/prefixdb.cpp
  prefixdb/domain/storage/multidb.cpp
  prefixdb/domain/storage/wrocksdb/wrocksdb_factory.cpp
  prefixdb/domain/storage/wrocksdb/wrocksdb.cpp
  prefixdb/domain/storage/wrocksdb/wrocksdb_restore.cpp
  prefixdb/domain/storage/wrocksdb/wrocksdb_slave.cpp
  prefixdb/domain/storage/wrocksdb/since_reader.cpp
  #prefixdb/domain/storage/wrocksdb/wal_buffer.cpp
  prefixdb/domain/storage/wrocksdb/merge/merge_operator.cpp
  prefixdb/domain/storage/god.cpp
  
  prefixdb/domain/prefixdb_multiton.cpp
  prefixdb/service/prefixdb_service_multiton.cpp
  prefixdb/service/prefixdb_cmd.cpp
  prefixdb/gateway/prefixdb_gateway_multiton.cpp
  prefixdb/domain/storage/aux/copy_dir.cpp
)

add_library(
  wfc_prefixdb
  ${prefixdb_source_list}
)

link_directories(${CMAKE_SOURCE_DIR}/luajit-2.0/src)
target_link_libraries(wfc_prefixdb rocksdb luajit )

add_dependencies(wfc_prefixdb luajit20 facebook-rocksdb )

build_info(wfc_prefixdb prefixdb)

install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})