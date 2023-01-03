# New in version 3.19: An interface library target may be created with source files:
cmake_minimum_required(VERSION 3.19)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(losync)

include(cmake/compiler.cmake)

add_subdirectory(src)

option(LOSYNC_ENABLE_BENCHMARKS "Build Losync benchmark binaries" OFF)
if(LOSYNC_ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

option(LOSYNC_ENABLE_TESTS "Build Losync test binaries" OFF)
if(LOSYNC_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
