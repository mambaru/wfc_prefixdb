
update_ogenc(TARGETS wfc_prefixdb WARNINGS -Wnull-dereference -Wno-redundant-tags OFF)

SET(
  prefixdb_source_list
    prefixdb_package.cpp
    prefixdb/prefixdb_module.cpp
    prefixdb/domain/prefixdb_domain.cpp
    prefixdb/domain/multidb/multidb.cpp
    prefixdb/domain/multidb/wrocksdb/wrocksdb_factory.cpp
    prefixdb/domain/multidb/wrocksdb/wrocksdb.cpp
    prefixdb/domain/multidb/wrocksdb/wrocksdb_restore.cpp
    prefixdb/domain/multidb/wrocksdb/wrocksdb_slave.cpp
    prefixdb/domain/multidb/wrocksdb/wrocksdb_initial.cpp
    prefixdb/domain/multidb/wrocksdb/since_reader.cpp
    prefixdb/domain/multidb/wrocksdb/merge/merge_operator.cpp
    prefixdb/domain/multidb/god.cpp
    prefixdb/domain/multidb/aux/scan_dir.cpp
    prefixdb/domain/multidb/aux/copy_dir.cpp
    prefixdb/domain/prefixdb_multiton.cpp
    prefixdb/service/prefixdb_service_multiton.cpp
    prefixdb/service/prefixdb_cmd.cpp
    prefixdb/gateway/prefixdb_gateway_multiton.cpp
)

add_library(wfc_prefixdb ${prefixdb_source_list})

wci_targets(wfc_prefixdb)
enable_stat(wfc_prefixdb)


# отключение предупреждений rocksdb параноидального режима
set_source_files_properties(
  prefixdb/domain/multidb/wrocksdb/wrocksdb_factory.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion -Wno-switch-default -Wno-unused-const-variable"
)


set_source_files_properties(
  prefixdb/domain/multidb/wrocksdb/wrocksdb.cpp
  prefixdb/domain/multidb/wrocksdb/wrocksdb_restore.cpp
  prefixdb/domain/multidb/wrocksdb/wrocksdb_slave.cpp
  prefixdb/domain/multidb/wrocksdb/wrocksdb_initial.cpp
  prefixdb/domain/multidb/wrocksdb/since_reader.cpp
  prefixdb/domain/multidb/multidb.cpp
  prefixdb/domain/multidb/god.cpp
  prefixdb/domain/prefixdb.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-sign-conversion -Wno-unused-const-variable"
)

set_source_files_properties(
  prefixdb/domain/multidb/wrocksdb/merge/merge_operator.cpp
  PROPERTIES
  COMPILE_FLAGS "-Wno-strict-overflow"
)

target_include_directories(wfc_prefixdb PUBLIC  "$<BUILD_INTERFACE:${wfc_prefixdb_SOURCE_DIR}>")
target_include_directories(wfc_prefixdb PUBLIC "$<BUILD_INTERFACE:${wfc_prefixdb_SOURCE_DIR}/package>")

find_library(LIBRT rt)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(wfc_prefixdb PUBLIC wfc rocksdb ${LIBRT} Threads::Threads ${SNAPPY_LIBRARIES})

add_dependencies(wfc_prefixdb rocksdb)
build_info(wfc_prefixdb prefixdb)
install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})
