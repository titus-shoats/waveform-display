cmake_minimum_required(VERSION 3.15)
project(JustASampleWin32 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    Win32/main.cpp
    Win32/AudioProcessor.cpp
    Win32/GUIComponents.cpp
    Win32/SamplerProcessor.cpp
)

set(HEADERS
    Win32/AudioProcessor.h
    Win32/GUIComponents.h
    Win32/SamplerProcessor.h
)

# Create executable
add_executable(JustASampleWin32 WIN32 ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(JustASampleWin32 
    PRIVATE
    winmm.lib      # For MIDI and multimedia
    ole32.lib      # For COM
    comctl32.lib   # For common controls
)

# Set runtime library to static
if(MSVC)
    set_property(TARGET JustASampleWin32 PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Compiler definitions
    target_compile_definitions(JustASampleWin32 PRIVATE
        UNICODE          # Use Unicode (wide character) Windows API
        _UNICODE         # Use Unicode C runtime
        NOMINMAX         # Prevent Windows.h from defining min/max macros
    )
    
    # Compiler flags for best practices
    target_compile_options(JustASampleWin32 PRIVATE
        /W4           # Warning level 4
        /WX-          # Don't treat warnings as errors (for now)
        /MP           # Multi-processor compilation
        /Zi           # Debug information
        /permissive-  # Standards conformance
    )
    
    # Linker flags
    target_link_options(JustASampleWin32 PRIVATE
        /SUBSYSTEM:WINDOWS
        /DEBUG
    )
endif()

# Set output directory
set_target_properties(JustASampleWin32 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Install
install(TARGETS JustASampleWin32
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
