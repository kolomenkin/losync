cmake_minimum_required(VERSION 3.17)

project(losync)

include(cmake/compiler.cmake)

add_subdirectory(src)

option(LOSYNC_ENABLE_BENCHMARKS "Build Losync benchmark binaries" OFF)
if(LOSYNC_ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

option(LOSYNC_ENABLE_TESTS "Build Losync test binaries" OFF)
if(LOSYNC_ENABLE_TESTS)
    add_subdirectory(tests)
endif()
