file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(graphics STATIC ${SOURCES})

find_package(directxtex CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# FFX stuff
set(FFX_API_VK OFF)
set(FFX_API_DX12 OFF)
set(FFX_ALL OFF)
set(FFX_FSR3 ON)
set(FFX_FSR ON)
set(FFX_AUTO_COMPILE_SHADERS 1)

add_subdirectory(${CMAKE_SOURCE_DIR}/extern/FidelityFX-SDK/sdk "${CMAKE_CURRENT_BINARY_DIR}/FFX")

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
        ffx_backend_dx11_x64
        ffx_fsr3_x64
)

target_precompile_headers(graphics PUBLIC "${CMAKE_SOURCE_DIR}/src/PCH.h" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h")

target_include_directories(graphics
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)