file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(util STATIC ${SOURCES})

target_link_libraries(util PUBLIC spdlog::spdlog)

target_include_directories(util
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)