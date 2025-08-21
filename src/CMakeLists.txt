file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE PCH.h)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(util)
add_subdirectory(window)
add_subdirectory(gfx)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        util
        platform
        graphics
)


foreach(tgt platform graphics ${PROJECT_NAME})
    target_compile_definitions(${tgt} PRIVATE
        DEBUG=$<$<CONFIG:Debug>:1>$<$<CONFIG:Release>:0>
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
endforeach()

add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_BINARY_DIR}/shaders"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    DEPENDS Shaders
)

set(SRC_ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(BIN_ASSETS_DIR "$<TARGET_FILE_DIR:game_engine>/assets")

# Copy assets every build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SRC_ASSETS_DIR}"
            "${BIN_ASSETS_DIR}"
    COMMENT "Copying assets to binary directory"
)