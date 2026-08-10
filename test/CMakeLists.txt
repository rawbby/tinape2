set(TEST_SOURCES
        common.cpp
        signed.cpp
        unsigned.cpp)

foreach (TEST_SOURCE IN LISTS TEST_SOURCES)
    set(TEST_SOURCE "${CMAKE_CURRENT_LIST_DIR}/${TEST_SOURCE}")

    file(RELATIVE_PATH TEST_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${TEST_SOURCE}")
    get_filename_component(TEST_REL_DIR "${TEST_REL_PATH}" DIRECTORY)
    get_filename_component(TEST_NAME_WE ${TEST_SOURCE} NAME_WE)

    string(TOLOWER "test-${TEST_REL_DIR}-${TEST_NAME_WE}" TEST_NAME)
    string(REPLACE "_" "-" TEST_NAME "${TEST_NAME}")
    string(REPLACE "/" "-" TEST_NAME "${TEST_NAME}")
    string(REGEX REPLACE "-+" "-" TEST_NAME "${TEST_NAME}")

    add_executable("${TEST_NAME}" "${TEST_SOURCE}")
    target_link_libraries("${TEST_NAME}" PRIVATE tinape)
    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
endforeach ()
