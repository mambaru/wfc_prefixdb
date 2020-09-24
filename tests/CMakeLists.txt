add_executable(test_prefixdb test_prefixdb.cpp prefixdb_suite.cpp)
target_link_libraries(test_prefixdb PRIVATE wfc_prefixdb rocksdb fas::testing)
add_dependencies(test_prefixdb wfc_prefixdb)
wci_test(TARGET test_prefixdb COMMAND test_prefixdb)
