# FL Studio Native Plugin + JUCE GUI - Step-by-Step Tutorial

This tutorial builds the FL Studio Native Plugin POC incrementally, testing in FL Studio at each step.

## Prerequisites

- **Visual Studio 2019/2022/2026** installed
- **CMake 3.22+** installed
- **JUCE 7.x+** downloaded to `C:/JUCE` (or adjust paths accordingly)
- **FL Studio 20.8+** installed for testing

---

## Step 1: Minimal FL Studio Plugin (No GUI)

### Goal
Create the simplest possible FL Studio plugin that loads without errors.

### ⚠️ CRITICAL NOTES
- **Message IDs Matter**: `FPD_GetPluginInfo` MUST be 46, not 4. Using wrong IDs causes crashes!
- **All Dispatcher Messages**: Handle all messages sent during initialization (SetSampleRate, SetBlockSize, Reset, SetEnabled)
- **Pointer Casting**: Use `intptr_t` for proper 64-bit pointer-to-int conversions
- **Testing**: Always restart FL Studio after copying new DLL

### Files to Create

#### 1.1 Create Directory Structure
```
FLStudioNative_POC_Step1/
├── CMakeLists.txt
├── PluginExport.def
├── SDK/
│   ├── fp_def.h
│   └── fp_plugclass.h
└── Source/
    ├── PluginEntry.cpp
    └── PluginProcessor.cpp
```

#### 1.2 SDK/fp_def.h
```cpp
#pragma once

// Prevent Windows min/max macro conflicts
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

// Critical dispatcher messages for Step 1
#define FPD_SetEnabled          0
#define FPD_SetSampleRate       1
#define FPD_SetBlockSize        2
#define FPD_Reset               3
#define FPD_ShowEditor          4
#define FPD_GetPluginInfo       46  // IMPORTANT: Must be 46, not 4!

// Plugin flags  
#define FPF_Type            0x0003  // Effect type (bits 0-1)

// Parameter flags
#define REC_UpdateValue     (1 << 0)
#define REC_ShowHint        (1 << 3)
```

#### 1.3 SDK/fp_plugclass.h
```cpp
#pragma once

#include "fp_def.h"

// Calling convention - define before use
#define PLUGIN_CALL __stdcall

// Forward declarations
struct TFruityPlugInfo;
struct TFruityPlugHost;

// Type definitions - must come before class
typedef void* PWAV32FS;

// Plugin info structure
struct TFruityPlugInfo
{
    int SDKVersion;
    char* LongName;
    char* ShortName;
    int NumParams;
    int Flags;
};

// Host callback structure
struct TFruityPlugHost
{
    void* HostTag;
    int (PLUGIN_CALL *Dispatcher)(void* HostTag, int Index, int Value, void* Ptr, int PtrSize);
};

// Base plugin class
class TFruityPlug
{
public:
    virtual ~TFruityPlug() {}
    
    virtual void DestroyObject() = 0;
    virtual int Dispatcher(int Index, int Value, void* Ptr, int PtrSize) = 0;
    virtual void Idle() {}
    virtual void SaveRestoreState(void* Stream, int Save) {}
    virtual void Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length) {}
    virtual int ProcessParam(int Index, int Value, int Flags) { return 0; }
};
```

#### 1.4 Source/PluginProcessor.cpp
```cpp
#include "../SDK/fp_plugclass.h"
#include <cstring>

class MinimalProcessor : public TFruityPlug
{
private:
    TFruityPlugInfo Info;
    TFruityPlugHost* Host;
    int Tag;
    
    char LongNameBuffer[256];
    char ShortNameBuffer[64];
    
public:
    MinimalProcessor(int PlugTag, TFruityPlugHost* PlugHost)
        : Tag(PlugTag)
        , Host(PlugHost)
    {
        memset(&Info, 0, sizeof(Info));
        memset(LongNameBuffer, 0, sizeof(LongNameBuffer));
        memset(ShortNameBuffer, 0, sizeof(ShortNameBuffer));
        
        Info.SDKVersion = 1;
        Info.LongName = LongNameBuffer;
        Info.ShortName = ShortNameBuffer;
        
        strncpy(Info.LongName, "Minimal FL Plugin", 255);
        strncpy(Info.ShortName, "MinPlug", 63);
        
        Info.NumParams = 0;
        Info.Flags = FPF_Type;  // Effect plugin
    }
    
    virtual ~MinimalProcessor() {}
    
    void DestroyObject() override
    {
        delete this;
    }
    
    int Dispatcher(int Index, int Value, void* Ptr, int PtrSize) override
    {
        switch (Index)
        {
            case FPD_GetPluginInfo:
            {
                // CRITICAL: Return pointer to plugin info
                // FL Studio needs this to identify the plugin
                return static_cast<int>(reinterpret_cast<intptr_t>(&Info));
            }
            
            case FPD_SetSampleRate:
            {
                // Sample rate notification - store for later use
                // Value contains sample rate (e.g., 44100, 48000)
                return 0;
            }
            
            case FPD_SetBlockSize:
            {
                // Block size notification - store for later use
                // Value contains buffer size (e.g., 512, 1024)
                return 0;
            }
            
            case FPD_Reset:
            {
                // Reset plugin state when FL Studio resets
                return 0;
            }
            
            case FPD_SetEnabled:
            {
                // Plugin enabled/disabled notification
                // Value: 0 = disabled, 1 = enabled
                return 0;
            }
            
            default:
                return 0;
        }
    }
    
    void Idle() override
    {
        // Empty for now - will pump messages in later steps
    }
    
    void SaveRestoreState(void* Stream, int Save) override
    {
        // Empty for now - no state to save yet
    }
    
    void Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length) override
    {
        // Empty for now - no audio processing yet
        // Just pass through (copy source to dest)
        if (SourceBuffer && DestBuffer && SourceBuffer != DestBuffer)
        {
            memcpy(DestBuffer, SourceBuffer, Length * 2 * sizeof(float));
        }
    }
    
    int ProcessParam(int Index, int Value, int Flags) override
    {
        // Empty for now - no parameters yet
        return 0;
    }
};

extern "C" __declspec(dllexport) TFruityPlug* _stdcall CreatePlugInstance(
    TFruityPlugHost* Host, int Tag)
{
    return new MinimalProcessor(Tag, Host);
}
```

#### 1.5 Source/PluginEntry.cpp
```cpp
// This file intentionally empty - all code is in PluginProcessor.cpp
// Created to match the project structure
```

#### 1.6 PluginExport.def
```
LIBRARY "FLStudioNative_POC_Step1"
EXPORTS
    CreatePlugInstance
```

#### 1.7 CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.22)
project(FLStudioNative_POC_Step1 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FL Studio SDK
add_library(fl_sdk INTERFACE)
target_include_directories(fl_sdk INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK
)

# Create the plugin DLL
add_library(FLStudioNative_POC_Step1 SHARED
    Source/PluginProcessor.cpp
    Source/PluginEntry.cpp
)

target_link_libraries(FLStudioNative_POC_Step1 PRIVATE
    fl_sdk
)

# DLL properties
set_target_properties(FLStudioNative_POC_Step1 PROPERTIES
    OUTPUT_NAME "FLStudioNative_POC_Step1"
    SUFFIX ".dll"
    PREFIX ""
)

# Export definition file
if(MSVC)
    set_target_properties(FLStudioNative_POC_Step1 PROPERTIES
        LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/PluginExport.def"
    )
endif()
```

### Build Instructions

```bash
cd FLStudioNative_POC_Step1
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

**Expected Result**: Build completes with 0 errors.

### FL Studio Testing

1. Copy `Release\FLStudioNative_POC_Step1.dll` to `C:\Program Files\Image-Line\FL Studio 2025\Plugins\Fruity\`
   (Adjust path for your FL Studio version)
2. **IMPORTANT**: Restart FL Studio completely
3. Open the mixer → Add effect → Fruity → Look for "Minimal FL Plugin"
4. Add the plugin

**Expected Result**: 
- Plugin appears in the list
- Loads without crash
- No editor window (clicking does nothing yet)
- Can be removed without crash

**Troubleshooting**:
- **If FL Studio crashes**: Check that `FPD_GetPluginInfo` is defined as 46, not 4
- **If plugin doesn't appear**: Verify DLL is in correct Plugins/Fruity/ directory
- **If "Access Violation" error**: Ensure all Dispatcher cases return valid values

**✅ CHECKPOINT**: If plugin loads successfully, proceed to Step 2.

---

## Step 2: Add Empty Win32 Window

### Goal
Add a simple Win32 window that opens when you click the plugin.

### Files to Modify/Add

#### 2.1 Modify Source/PluginProcessor.cpp

Replace entire file with:

```cpp
#include "../SDK/fp_plugclass.h"
#include <cstring>

// Win32 window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
            
        case WM_DESTROY:
            return 0;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

class Step2Processor : public TFruityPlug
{
private:
    TFruityPlugInfo Info;
    TFruityPlugHost* Host;
    int Tag;
    
    char LongNameBuffer[256];
    char ShortNameBuffer[64];
    
    HWND editorWindow;
    
public:
    Step2Processor(int PlugTag, TFruityPlugHost* PlugHost)
        : Tag(PlugTag)
        , Host(PlugHost)
        , editorWindow(nullptr)
    {
        memset(&Info, 0, sizeof(Info));
        memset(LongNameBuffer, 0, sizeof(LongNameBuffer));
        memset(ShortNameBuffer, 0, sizeof(ShortNameBuffer));
        
        Info.SDKVersion = 1;
        Info.LongName = LongNameBuffer;
        Info.ShortName = ShortNameBuffer;
        
        strncpy(Info.LongName, "Step 2 - Window", 255);
        strncpy(Info.ShortName, "Step2", 63);
        
        Info.NumParams = 0;
        Info.Flags = FPF_Type;
    }
    
    virtual ~Step2Processor()
    {
        if (editorWindow)
        {
            DestroyWindow(editorWindow);
        }
    }
    
    void DestroyObject() override
    {
        delete this;
    }
    
    int Dispatcher(int Index, int Value, void* Ptr, int PtrSize) override
    {
        switch (Index)
        {
            case FPD_GetPluginInfo:
                return static_cast<int>(reinterpret_cast<intptr_t>(&Info));
                
            case FPD_ShowEditor:
            {
                HWND parentWindow = reinterpret_cast<HWND>(Ptr);
                
                if (Value == 0)  // Hide
                {
                    if (editorWindow)
                    {
                        ShowWindow(editorWindow, SW_HIDE);
                    }
                }
                else  // Show
                {
                    if (!editorWindow)
                    {
                        createEditorWindow(parentWindow);
                    }
                    
                    if (editorWindow)
                    {
                        ShowWindow(editorWindow, SW_SHOW);
                        SetFocus(editorWindow);
                    }
                }
                return 1;
            }
                
            default:
                return 0;
        }
    }
    
    void Idle() override
    {
        // Process Windows messages for our window
        if (editorWindow)
        {
            MSG msg;
            while (PeekMessage(&msg, editorWindow, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    
private:
    void createEditorWindow(HWND parentWindow)
    {
        // Register window class
        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "Step2Window";
        
        static bool registered = false;
        if (!registered)
        {
            RegisterClassExA(&wc);
            registered = true;
        }
        
        // Create window
        editorWindow = CreateWindowExA(
            0,
            "Step2Window",
            "Step 2 - Empty Window",
            WS_CHILD | WS_VISIBLE,
            0, 0, 600, 400,
            parentWindow,
            nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );
    }
};

extern "C" __declspec(dllexport) TFruityPlug* _stdcall CreatePlugInstance(
    TFruityPlugHost* Host, int Tag)
{
    return new Step2Processor(Tag, Host);
}
```

### Build Instructions

```bash
cd build
cmake --build . --config Release
```

**Expected Result**: Build completes with 0 errors.

### FL Studio Testing

1. Copy new DLL to FL Studio plugins folder
2. Restart FL Studio (or use plugin rescanner)
3. Add "Step 2 - Window" plugin to mixer
4. Click the plugin to open editor

**Expected Result**:
- Empty white window appears (600x400)
- Window title shows "Step 2 - Empty Window"
- Window can be closed (hides, doesn't crash)
- Plugin can be removed without crash

**✅ CHECKPOINT**: If window opens/closes successfully, proceed to Step 3.

---

## Step 3: Add JUCE Initialization

Continue with remaining steps in next document section...

Would you like me to continue with Steps 3-7, or would you prefer to test Steps 1-2 first?
