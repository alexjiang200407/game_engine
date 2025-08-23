
set(SOURCES
    src/bindings/Bindable.cpp
    src/bindings/IndexBuffer.cpp
    src/bindings/InputLayout.cpp
    src/bindings/PixelShader.cpp
    src/bindings/Sampler.cpp
    src/bindings/Texture.cpp
    src/bindings/Topology.cpp
    src/bindings/TransformCBuffer.cpp
    src/bindings/VertexBuffer.cpp
    src/bindings/VertexShader.cpp
    src/draw/Box.cpp
    src/draw/Drawable.cpp
    src/draw/DrawableFactory.cpp
    src/draw/Melon.cpp
    src/draw/Pyramid.cpp
    src/draw/Sheet.cpp
    src/DXGIInfoManager.cpp
    src/GFXException.cpp
    src/Graphics.cpp
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

target_precompile_headers(graphics PUBLIC "${CMAKE_SOURCE_DIR}/src/PCH.h" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h")

target_include_directories(graphics
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)