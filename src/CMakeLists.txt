set(CORE_SOURCES
    logger.cpp
    main.cpp
    Game.cpp
)

add_executable(${PROJECT_NAME} WIN32
    ${CORE_SOURCES}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE PCH.h)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

add_subdirectory(window)