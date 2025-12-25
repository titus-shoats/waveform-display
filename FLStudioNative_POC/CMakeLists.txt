# CMakeLists.txt
# FL Studio Native + JUCE GUI POC - Build Configuration
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

# JUCE Setup
# Option 1: JUCE as a subdirectory (recommended)
# Uncomment and adjust path if JUCE is a submodule or in a subdirectory
# add_subdirectory(../JUCE ./juce-build)

# Option 2: JUCE installed via package manager
# find_package(JUCE CONFIG REQUIRED)

# For this POC, we'll assume JUCE is available via the parent project
# If building standalone, you'll need to add JUCE via one of the options above

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
# Note: You may need to adjust these based on your JUCE installation
target_link_libraries(FLStudioNative_POC PRIVATE
    fl_sdk
    # Uncomment these if using juce_add_plugin or if JUCE is available:
    # juce::juce_core
    # juce::juce_audio_basics
    # juce::juce_audio_processors
    # juce::juce_gui_basics
    # juce::juce_gui_extra
    # juce::juce_events
)

# Include JUCE headers
# Adjust this path based on your JUCE installation
target_include_directories(FLStudioNative_POC PRIVATE
    # Example paths - adjust for your setup:
    # ${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/modules
    # Or if JUCE is installed system-wide, it may be found automatically
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
