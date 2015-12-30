set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WFC_TEST_DIR})

SET(
  prefixdb_test_source_list
  leveldb_suite.cpp
  unit_test.cpp
  perf_test.cpp
  ${FAS_TESTING_CPP}
)

add_executable(
  prefixdb_test
  ${prefixdb_test_source_list}
)

add_dependencies(prefixdb_test luajit20 google-leveldb facebook-rocksdb)

target_link_libraries(prefixdb_test 
  #boost_filesystem-mt 
  #boost_system-mt 
  luajit
  leveldb
  #leveldb_storage 
)

add_test(prefixdb_test prefixdb_test)


