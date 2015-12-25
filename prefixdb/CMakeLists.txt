
include(BuildInfo)

include_directories(.)

SET(
  prefixdb_source_list
  prefixdb_package.cpp
  prefixdb/prefixdb_module.cpp
  prefixdb/domain/prefixdb.cpp
  prefixdb/domain/impl/multidb.cpp
  prefixdb/domain/impl/rocksdb_factory.cpp
  prefixdb/domain/impl/rocksdb1_factory.cpp
  #prefixdb/domain/impl/rocksdb.cpp
  prefixdb/domain/impl/rocksdb1.cpp
  prefixdb/domain/impl/god.cpp
  prefixdb/domain/prefixdb_multiton.cpp
  prefixdb/service/prefixdb_service_multiton.cpp
  prefixdb/gateway/prefixdb_gateway_multiton.cpp
)

add_library(
  wfc_prefixdb
  ${prefixdb_source_list}
)

target_link_libraries(wfc_prefixdb rocksdb leveldb luajit)
add_dependencies(wfc_prefixdb luajit20 google-leveldb facebook-rocksdb)

build_info(wfc_prefixdb prefixdb)

install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})