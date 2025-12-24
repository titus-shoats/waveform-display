cmake_minimum_required(VERSION 3.15)

project(JustASample_Win32 VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(NOT WIN32)
    message(FATAL_ERROR "This project is designed for Windows only")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Stage 1: Basic Window Application
# We'll build this first to ensure the basic window works

set(STAGE1_SOURCES
    src/main.cpp
)

# Create the standalone executable
add_executable(JustASample_Win32 WIN32 ${STAGE1_SOURCES})

# Link Windows libraries
target_link_libraries(JustASample_Win32 PRIVATE
    comctl32
    gdi32
    user32
)

# Set subsystem to Windows (GUI application)
if(MSVC)
    set_target_properties(JustASample_Win32 PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
    )
endif()

# Group source files for IDE
source_group("Source Files" FILES ${STAGE1_SOURCES})

# Print build configuration
message(STATUS "==== Just a Sample Win32 Build Configuration ====")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Stage: 1 (Basic Window)")
message(STATUS "=================================================")

# Installation rules (optional)
install(TARGETS JustASample_Win32
    RUNTIME DESTINATION bin
)

# Future stages will be added below as we progress
# Stage 2: VST3 Plugin (TODO)
# Stage 3: Audio Processing (TODO)
# Stage 4: GUI Components (TODO)
