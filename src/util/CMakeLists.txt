
set(SOURCES
    src/logger.cpp
    src/Timer.cpp
)

add_library(util STATIC ${SOURCES})

target_link_libraries(util PUBLIC spdlog::spdlog)

target_include_directories(util
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)