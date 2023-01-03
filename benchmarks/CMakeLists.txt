add_subdirectory(../third_party/google_benchmark gbenchmark)

set(TARGET losync_benchmarks)

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
target_link_libraries(${TARGET} benchmark::benchmark_main)
