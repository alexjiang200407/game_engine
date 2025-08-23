
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(platform STATIC ${SOURCES})

target_link_libraries(platform
    PUBLIC
        util
)

target_include_directories(
    platform
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_precompile_headers(
    platform
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/PCH.h"
)