# Win32 Implementation - Step by Step Guide

This guide will walk you through implementing a Win32 version of the Just a Sample VST plugin, step by step. After each step, we'll build and test to ensure everything works before moving forward.

---

## Prerequisites

Before starting, ensure you have:

1. **Windows 10 or later**
2. **Visual Studio 2017 or later** (Community Edition is free)
   - Download from: https://visualstudio.microsoft.com/downloads/
   - During installation, select "Desktop development with C++"
3. **CMake 3.15 or later**
   - Download from: https://cmake.org/download/
   - Add to PATH during installation
4. **Git** (to clone the repository)

---

## Stage 1: Basic Window Application

### Step 1.1: Verify the Files

Ensure these files exist in `Win32Version/`:

```
Win32Version/
├── CMakeLists.txt
├── build.bat
├── src/
│   └── main.cpp
├── include/
├── resources/
└── README_WIN32.md
```

### Step 1.2: Build the Application

1. Open Command Prompt or PowerShell
2. Navigate to the `Win32Version` directory:
   ```cmd
   cd Win32Version
   ```
3. Run the build script:
   ```cmd
   build.bat
   ```

**Expected Output:**
- CMake should configure successfully
- Visual Studio project should be generated
- Compilation should complete with 0 errors

**If build fails:**
- Check that CMake is in your PATH: `cmake --version`
- Check Visual Studio installation
- Try manual build: see "Manual Build Instructions" below

### Step 1.3: Run the Application

1. Navigate to the output directory:
   ```cmd
   cd build\bin\Release
   ```
2. Run the executable:
   ```cmd
   JustASample_Win32.exe
   ```

**Expected Result:**
- A window should open with title "Just a Sample - Win32 Version"
- Window size should be approximately 900x600 pixels
- Window should display: "Just a Sample - Win32 Version" and "Stage 1: Basic Window (COMPLETE)"
- Window should have a light gray background with a border
- Closing the window should show a confirmation dialog

**Test Checklist:**
- [ ] Window opens successfully
- [ ] Window is resizable
- [ ] Window displays text
- [ ] Minimize/Maximize buttons work
- [ ] Close button shows confirmation dialog
- [ ] Window closes properly when confirmed

### Step 1.4: Understanding the Code

**main.cpp** contains:

1. **WinMain**: Entry point for Windows GUI applications
   - Initializes common controls
   - Registers window class
   - Creates main window
   - Runs message loop

2. **WndProc**: Window procedure that handles messages
   - `WM_CREATE`: Called when window is created
   - `WM_PAINT`: Called when window needs repainting
   - `WM_SIZE`: Called when window is resized
   - `WM_CLOSE`: Called when user closes window
   - `WM_DESTROY`: Called when window is destroyed

3. **Key Variables:**
   - `g_szClassName`: Window class name = "JustASampleWindow"
   - `g_szWindowTitle`: Window title = "Just a Sample - Win32 Version"
   - `WINDOW_WIDTH`: 900 pixels
   - `WINDOW_HEIGHT`: 600 pixels

---

## Manual Build Instructions

If `build.bat` doesn't work, try these manual steps:

### Using Visual Studio 2022:
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Using Visual Studio 2019:
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### Using MinGW:
```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

---

## Troubleshooting

### Error: "CMake not found"
- Install CMake from https://cmake.org/download/
- Make sure to add CMake to PATH during installation
- Restart Command Prompt after installation

### Error: "Visual Studio generator not found"
- Install Visual Studio with "Desktop development with C++" workload
- If using older VS, change the generator in build.bat or manual commands

### Error: "Cannot open include file 'windows.h'"
- Ensure Windows SDK is installed with Visual Studio
- In Visual Studio Installer, modify installation and add Windows SDK

### Window doesn't appear
- Check Windows Task Manager if process is running
- Try running from Command Prompt to see error messages
- Check antivirus isn't blocking the executable

---

## Next Steps

Once Stage 1 is working, we'll proceed to:

**Stage 2: Plugin Editor Window**
- Create a custom window class for the plugin GUI
- Add basic controls (buttons, sliders, labels)
- Implement parameter display
- Test control interaction

**After Stage 2 verification, we'll continue to:**
- Stage 3: VST3 Plugin Shell
- Stage 4: Audio Processing
- Stage 5: Waveform Display
- And so on...

---

## Verification Before Moving On

Before proceeding to Stage 2, confirm:

1. ✅ Application builds without errors
2. ✅ Application runs without crashes
3. ✅ Window opens and displays correctly
4. ✅ Window controls (minimize, maximize, close) work
5. ✅ You understand the basic Win32 window structure

**Once all checks pass, you're ready for Stage 2!**

---

## Notes

- Keep the application running when testing - it should be stable
- Try resizing the window - it should handle this gracefully
- The current code is the foundation; we'll build on it incrementally
- Each file and variable name matches the original plugin's architecture where possible

## Questions?

If something isn't working:
1. Check the Troubleshooting section above
2. Verify all prerequisites are installed
3. Try manual build commands
4. Check error messages carefully

**Ready to continue? Let me know when Stage 1 is verified!**
