
include(BuildInfo)

include_directories(.)

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

target_link_libraries(wfc_prefixdb rocksdb ${WFC_LIBRARIES})

add_dependencies(wfc_prefixdb rocksdb )

build_info(wfc_prefixdb prefixdb)

install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})
