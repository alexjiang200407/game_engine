
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(util STATIC ${SOURCES})

target_link_libraries(util)

target_include_directories(util
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)