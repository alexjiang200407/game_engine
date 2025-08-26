file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(graphics STATIC ${SOURCES})

find_package(directxtex CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_link_libraries(graphics
    PUBLIC
        platform
        util
        imgui::imgui
    PRIVATE
        d3d11
        dxgi
        d3dcompiler
        dxguid
        Microsoft::DirectXTex
        assimp::assimp
)

target_precompile_headers(graphics PUBLIC "${CMAKE_SOURCE_DIR}/src/PCH.h" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h")

target_include_directories(graphics
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)