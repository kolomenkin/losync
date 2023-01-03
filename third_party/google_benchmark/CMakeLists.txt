# CUSTOM CMake file for Losync project:
set(CMAKE_FOLDER "third_party")

set(PROJECT_NAME "GBenchamark")
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(GENERIC_LIB_VERSION "1.7.1")
string(SUBSTRING ${GENERIC_LIB_VERSION} 0 1 GENERIC_LIB_SOVERSION)

# Ensure we have pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_subdirectory(src)
