add_subdirectory(../third_party/google_benchmark gbenchmark)

set(CMAKE_FOLDER "${LOSYNC_FOLDER}benchmarks")

file(GLOB ALL_SOURCES *.cpp)

foreach(file ${ALL_SOURCES})
    STRING(REGEX REPLACE "^.+/([^/]+)\\.cpp$" "LosyncBm\\1" TARGET ${file})

    file(GLOB HEADERS *.h)
    set(SOURCES ${file})

    set(CI_FILES
        CMakeLists.txt
    )

    add_executable(${TARGET} ${HEADERS} ${SOURCES} ${CI_FILES})

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Headers" FILES ${HEADERS})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CI Files" FILES ${CI_FILES})

    target_link_libraries(${TARGET} losync::losync)
    target_link_libraries(${TARGET} benchmark::benchmark_main)
endforeach()
