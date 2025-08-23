
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(platform STATIC ${SOURCES})

find_package(imgui CONFIG REQUIRED)

target_link_libraries(platform
    PUBLIC
        imgui::imgui
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