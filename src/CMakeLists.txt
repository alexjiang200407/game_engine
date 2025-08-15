file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)


add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE PCH.h)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
        d3d11
        dxgi
        d3dcompiler
)

