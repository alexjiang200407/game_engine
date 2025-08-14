
set(WINDOW_SOURCES
    Keyboard.cpp
    Mouse.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${WINDOW_SOURCES})
add_subdirectory(win32)