# CUSTOM CMake file for Losync project:

set(TARGET gtest)

file(GLOB_RECURSE HEADERS include/gtest/*.h)

file(GLOB SOURCES src/gtest.cc src/gtest-*.cc src/gtest-*.h)

set(CI_FILES
    CMakeLists.txt
)

add_library(${TARGET} ${HEADERS} ${SOURCES} ${CI_FILES})
add_library(gtest::gtest ALIAS ${TARGET})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/gtest PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CI Files" FILES ${CI_FILES})

set_source_files_properties(${SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties(src/gtest-all.cc PROPERTIES HEADER_FILE_ONLY FALSE)

target_include_directories(${TARGET} PUBLIC include)
target_include_directories(${TARGET} PRIVATE .)

# ===============================================

set(TARGET gtest_main)

set(HEADERS
)

set(SOURCES
    src/gtest_main.cc
)

set(CI_FILES
    CMakeLists.txt
)

add_library(${TARGET} ${HEADERS} ${SOURCES} ${CI_FILES})
add_library(gtest::gtest_main ALIAS ${TARGET})

target_link_libraries(${TARGET} gtest)
