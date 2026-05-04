cmake_minimum_required(VERSION 3.15)
project(Amazons-local VERSION 1.0.0 LANGUAGES CXX RC)

# C++17 required (EGE and project code use C++17 features)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # gnu++17

# ---- EGE Graphics Library (bundled) ----
set(EGE_DIR "${CMAKE_SOURCE_DIR}/lib")
set(EGE_INC  "${EGE_DIR}/include")
set(EGE_LIB  "${EGE_DIR}/libgraphics.a")

# ---- Source files ----
set(SOURCES
    main.cpp
    Game.cpp
    Game_board.cpp
    AI_Kernel.cpp
    Renderer.cpp
    SaveManager.cpp
    platform/EGEPlatform.cpp
)

set(HEADERS
    Constants.h
    Game.h
    Game_board.h
    AI_Kernel.h
    Renderer.h
    SaveManager.h
    platform/IPlatform.h
    platform/EGEPlatform.h
    platform/KeyCode.h
)

# ---- Windows resource (icon) ----
set(RESOURCE_FILES
    Amazons-local_private.rc
    app.ico
)

# ---- Executable ----
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCE_FILES}
)

# ---- Include directories ----
target_include_directories(${PROJECT_NAME} PRIVATE
    ${EGE_INC}
    ${CMAKE_SOURCE_DIR}
)

# ---- Compiler flags (matching original makefile) ----
target_compile_options(${PROJECT_NAME} PRIVATE
    -O2
    -pipe
    -fno-ms-extensions
    -Werror=return-type
    -Werror=vla
)

# ---- Link EGE and Windows system libraries ----
target_link_directories(${PROJECT_NAME} PRIVATE ${EGE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${EGE_LIB}
    uuid
    msimg32
    gdi32
    imm32
    ole32
    oleaut32
    winmm
    gdiplus
    -static
    -mwindows
)

# ---- Linker flags ----
target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--stack,12582912
    -s
)

# ---- Copy music.wav to output directory (if present) ----
if(EXISTS "${CMAKE_SOURCE_DIR}/music.wav")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/music.wav"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/music.wav"
        COMMENT "Copying music.wav to output directory"
    )
endif()
