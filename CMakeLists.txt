cmake_minimum_required(VERSION 3.25)
project(tinape CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TINAPE_LOCAL "Enable local tests and lint targets" ON)

add_library(tinape INTERFACE)
target_include_directories(tinape INTERFACE ${PROJECT_SOURCE_DIR}/include)

enable_testing()
add_subdirectory(test)

if (TINAPE_LOCAL)
    file(GLOB_RECURSE LINT_SOURCES ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/test/*.cpp)
    file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp)
    find_program(CLANG_FORMAT NAMES clang-format-20 clang-format-21 clang-format-22)
    find_program(CLANG_TIDY NAMES clang-tidy-20 clang-tidy-21 clang-tidy-22)
    if (CLANG_FORMAT)
        add_custom_target(format COMMAND ${CLANG_FORMAT} -i ${LINT_SOURCES})
        add_custom_target(format-check COMMAND ${CLANG_FORMAT} --dry-run --Werror ${LINT_SOURCES})
    endif ()
    if (CLANG_TIDY)
        add_custom_target(tidy-check COMMAND ${CLANG_TIDY} -p ${CMAKE_BINARY_DIR} ${TEST_SOURCES})
    endif ()
endif ()
