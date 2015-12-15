
include(BuildInfo)

include_directories(.)

SET(
  prefixdb_source_list
  prefixdb_package.cpp
  prefixdb/prefixdb_module.cpp
  prefixdb/domain/prefixdb.cpp
  prefixdb/domain/prefixdb_multiton.cpp
)

add_library(
  wfc_prefixdb
  ${prefixdb_source_list}
)

target_link_libraries(wfc_prefixdb leveldb luajit)
add_dependencies(wfc_prefixdb luajit20 google-leveldb)

build_info(wfc_prefixdb prefixdb)

install(TARGETS wfc_prefixdb DESTINATION ${CMAKE_INSTALL_PREFIX})