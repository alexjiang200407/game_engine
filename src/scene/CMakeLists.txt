file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(scene STATIC ${SOURCES})

target_link_libraries(scene
    PUBLIC
        util
        imgui::imgui
)

target_precompile_headers(scene PUBLIC "${CMAKE_SOURCE_DIR}/src/PCH.h")

target_include_directories(scene
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)