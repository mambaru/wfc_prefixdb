
include(BuildInfo)

include_directories(.)

SET(
  prefixdb_source_list
  prefixdb_package.cpp
  prefixdb/prefixdb_module.cpp
  prefixdb/domain/prefixdb.cpp
  prefixdb/domain/storage/multidb.cpp
  prefixdb/domain/storage/rocksdb_factory.cpp
  prefixdb/domain/storage/rocksdb.cpp
  prefixdb/domain/storage/god.cpp
  prefixdb/domain/storage/merge/merge_operator.cpp
  prefixdb/domain/prefixdb_multiton.cpp
  prefixdb/service/prefixdb_service_multiton.cpp
  prefixdb/gateway/prefixdb_gateway_multiton.cpp
)

add_library(
  wfc_prefixdb
  ${prefixdb_source_list}
)

link_directories(${CMAKE_SOURCE_DIR}/luajit-2.0/src)
target_link_libraries(wfc_prefixdb rocksdb luajit)
#target_link_libraries(wfc_prefixdb rocksdb)
add_dependencies(wfc_prefixdb luajit20 facebook-rocksdb)

build_info(wfc_prefixdb prefixdb)

install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})