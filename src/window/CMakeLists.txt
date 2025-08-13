
set(WINDOW_SOURCES
    Keyboard.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${WINDOW_SOURCES})
add_subdirectory(win32)