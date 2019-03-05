set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})

add_executable(
  test_prefixdb
    test_prefixdb.cpp
    prefixdb_suite.cpp
    ${FAS_TESTING_CPP}
)

target_link_libraries(test_prefixdb wfc_prefixdb)
add_dependencies(test_prefixdb wfc_prefixdb)
add_test(NAME test_prefixdb COMMAND test_prefixdb)
