# CMakeLists.txt
# FL Studio Native + JUCE GUI POC - Build Configuration
#
# ⚠️ IMPORTANT: This CMakeLists.txt is a TEMPLATE ⚠️
# You MUST configure JUCE integration before building (see options below).
# Choose ONE of the JUCE setup options and uncomment the relevant lines.
#
# This CMake file demonstrates how to build an FL Studio native plugin
# that embeds a JUCE GUI. The key challenges are:
# - Linking JUCE framework properly
# - Creating a DLL with correct exports for FL Studio
# - Including FL Studio SDK headers
#
# Requirements:
# - CMake 3.22 or later
# - JUCE Framework 7.x or later (as submodule or find_package)
# - Visual Studio 2019/2022 (Windows only)

cmake_minimum_required(VERSION 3.22)
project(FLStudioNative_POC VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# JUCE Setup - Auto-detection with fallback options
# ============================================================================

# Try to find JUCE automatically
set(JUCE_FOUND FALSE)

# Option 1: Check if JUCE_DIR is provided via command line (e.g., -DJUCE_DIR=...)
if(DEFINED JUCE_DIR AND EXISTS "${JUCE_DIR}")
    message(STATUS "Using JUCE from JUCE_DIR: ${JUCE_DIR}")
    if(EXISTS "${JUCE_DIR}/modules")
        add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/juce-build)
        set(JUCE_FOUND TRUE)
    elseif(EXISTS "${JUCE_DIR}/CMakeLists.txt")
        add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/juce-build)
        set(JUCE_FOUND TRUE)
    else()
        message(WARNING "JUCE_DIR is set but doesn't contain valid JUCE installation")
    endif()
endif()

# Option 2: Check for JUCE in parent directory
if(NOT JUCE_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
    message(STATUS "Found JUCE in parent directory")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../JUCE ${CMAKE_BINARY_DIR}/juce-build)
    set(JUCE_FOUND TRUE)
endif()

# Option 3: Try find_package for system-wide installation
if(NOT JUCE_FOUND)
    find_package(JUCE CONFIG QUIET)
    if(JUCE_FOUND)
        message(STATUS "Found JUCE via find_package")
    endif()
endif()

# If JUCE still not found, provide helpful error message
if(NOT JUCE_FOUND)
    message(FATAL_ERROR 
        "JUCE not found! Please provide JUCE location using one of these methods:\n"
        "  1. cmake .. -DJUCE_DIR=/path/to/JUCE\n"
        "  2. Place JUCE in parent directory (../JUCE)\n"
        "  3. Install JUCE system-wide and it will be auto-detected\n"
        "\nExample: cmake .. -G \"Visual Studio 17 2022\" -A x64 -DJUCE_DIR=C:/path/to/JUCE"
    )
endif()

# ============================================================================

# FL Studio SDK - Interface library
add_library(fl_sdk INTERFACE)
target_include_directories(fl_sdk INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK
)

# Create the plugin as a shared library (DLL)
add_library(FLStudioNative_POC SHARED
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/PluginEntry.cpp
)

# Link libraries
target_link_libraries(FLStudioNative_POC PRIVATE
    fl_sdk
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_events
)

# Set output name and properties for FL Studio DLL
set_target_properties(FLStudioNative_POC PROPERTIES
    OUTPUT_NAME "FLStudioNative_POC"
    SUFFIX ".dll"
    PREFIX ""
)

# Add the export definition file
if(MSVC)
    set_target_properties(FLStudioNative_POC PROPERTIES
        LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/PluginExport.def"
    )
endif()

# Compiler definitions
target_compile_definitions(FLStudioNative_POC PRIVATE
    JUCE_STANDALONE_APPLICATION=0
    JUCE_PLUGINHOST_VST3=0
    JUCE_PLUGINHOST_AU=0
)

# Installation rules (optional)
# Install the DLL to FL Studio's plugin directory
# Adjust the path to your FL Studio installation
# install(TARGETS FLStudioNative_POC
#     RUNTIME DESTINATION "C:/Program Files/Image-Line/FL Studio/Plugins/Fruity"
# )

# Post-build message
add_custom_command(TARGET FLStudioNative_POC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete! Copy FLStudioNative_POC.dll to your FL Studio Plugins directory."
)
