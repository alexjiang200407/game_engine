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
        spdlog::spdlog
        platform
        graphics
)

set(MY_DEFINITIONS
    $<$<CONFIG:Debug>:DEBUG>
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

foreach(tgt platform graphics ${PROJECT_NAME})
    target_compile_definitions(${tgt} PUBLIC ${MY_DEFINITIONS})
endforeach()

add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_BINARY_DIR}/shaders"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    DEPENDS Shaders
)