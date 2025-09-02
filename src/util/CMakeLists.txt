file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(util STATIC ${SOURCES})

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

target_link_libraries(util PUBLIC spdlog::spdlog)

target_include_directories(util
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    	${SIMPLEINI_INCLUDE_DIRS}
)

target_precompile_headers(util PUBLIC "${CMAKE_SOURCE_DIR}/src/PCH.h")
