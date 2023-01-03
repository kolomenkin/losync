set(TARGET losync)

file(GLOB HEADERS ../include/losync/*.h)

set(SOURCES
    dummy.cpp
)

set(CI_FILES
    CMakeLists.txt
)

add_library(${TARGET} ${HEADERS} ${SOURCES} ${CI_FILES})
add_library(losync::losync ALIAS ${TARGET})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/losync PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "CI Files" FILES ${CI_FILES})

target_include_directories(${TARGET} PUBLIC ../include)
