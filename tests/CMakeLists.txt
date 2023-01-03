add_subdirectory(../third_party/google_test gtest)

set(TARGET losync_tests)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.cpp)

set(CI_FILES
    CMakeLists.txt
)

add_executable(${TARGET} ${HEADERS} ${SOURCES} ${CI_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CI Files" FILES ${CI_FILES})

target_link_libraries(${TARGET} losync::losync)
target_link_libraries(${TARGET} gtest::gtest_main)

add_test(NAME ${TARGET} COMMAND $<TARGET_FILE:${TARGET}>)
