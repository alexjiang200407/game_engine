
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bind/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/draw/*.cpp"
)

add_library(graphics STATIC ${SOURCES})

find_package(directxtex CONFIG REQUIRED)

target_link_libraries(graphics
    PUBLIC
        platform
        util
    PRIVATE
        d3d11
        dxgi
        d3dcompiler
        dxguid
        Microsoft::DirectXTex
)

target_precompile_headers(graphics PUBLIC "${CMAKE_SOURCE_DIR}/src/PCH.h")

target_include_directories(
    graphics
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE_HEADER
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
