set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})

add_executable(
  prefixdb_test
    prefixdb_suite.cpp
    test_prefixdb.cpp
    ${FAS_TESTING_CPP}
)

target_link_libraries(
  prefixdb_test 
    wfc_prefixdb
)

add_test(NAME prefixdb_test COMMAND prefixdb_test)


