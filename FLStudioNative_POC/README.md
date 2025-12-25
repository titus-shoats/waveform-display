# FL Studio Native Plugin + JUCE GUI Proof of Concept

## Purpose

This proof-of-concept demonstrates how to embed JUCE GUI components within an FL Studio native plugin. Unlike standard VST3/AU plugins, FL Studio native plugins use a proprietary SDK and require special integration techniques to work with modern GUI frameworks like JUCE.

This POC serves as a template for converting existing JUCE-based plugins (like waveform-display) to FL Studio native format while preserving the JUCE GUI and interactivity.

## What This Demonstrates

✅ **JUCE GUI Embedded in FL Studio** - Complete JUCE component hierarchy works inside FL Studio's window system  
✅ **Win32 Container Window** - Proper parent/child window relationship  
✅ **Message Loop Integration** - JUCE timers and events work via `Idle()` pumping  
✅ **Parameter Bridging** - Seamless conversion between FL Studio's 0-65535 int values and JUCE's 0.0-1.0 float values  
✅ **Timer Updates** - Proves JUCE `Timer` class works correctly (updates every second)  
✅ **Button Interactions** - Clickable buttons with event handlers  
✅ **Slider Controls** - Interactive slider that updates parameters  
✅ **File Chooser Dialogs** - Modal file dialogs work properly  
✅ **Basic Audio Processing** - Simple gain effect demonstrates audio path  

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│ FL Studio Host                                              │
│  ├─ Calls CreatePlugInstance()                              │
│  ├─ Calls Dispatcher() for editor show/hide                 │
│  ├─ Calls Idle() continuously (pumps JUCE message loop)     │
│  ├─ Calls ProcessParam() for parameter changes              │
│  └─ Calls Eff_Render() for audio processing                 │
└──────────────────┬──────────────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│ HelloWorldProcessor (TCPPFruityPlug)                        │
│  ├─ Implements FL Studio plugin interface                   │
│  ├─ Creates Win32 container window                          │
│  ├─ Manages JUCE editor lifecycle                           │
│  ├─ Bridges parameters (int ↔ float)                        │
│  └─ Processes audio with gain effect                        │
└──────────────────┬──────────────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│ Win32 Container Window (HWND)                               │
│  └─ Parent window for JUCE editor                           │
└──────────────────┬──────────────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│ HelloWorldEditor (juce::Component)                          │
│  ├─ Title label                                             │
│  ├─ Timer label (updates every second)                      │
│  ├─ Gain slider (bidirectional parameter sync)              │
│  ├─ Test button (proves event handling)                     │
│  ├─ File chooser button (proves modal dialogs)              │
│  └─ Status label (feedback for interactions)                │
└─────────────────────────────────────────────────────────────┘
```

## Requirements

### Build Tools
- **Visual Studio 2019 or 2022** (Community Edition or higher)
- **CMake 3.22+** ([Download here](https://cmake.org/download/))
- **Windows 10/11** (FL Studio is Windows-only)

### Dependencies
- **JUCE Framework 7.x+** ([GitHub](https://github.com/juce-framework/JUCE))
- **FL Studio SDK** (included as minimal headers in `SDK/` directory)

### Runtime
- **FL Studio 20.8+** (for testing the plugin)

## Project Structure

```
FLStudioNative_POC/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── PluginExport.def            # DLL export definitions
├── SDK/                        # FL Studio SDK headers (minimal)
│   ├── fp_def.h               # Constants and definitions
│   ├── fp_plugclass.h         # Base plugin class
│   ├── fp_cplug.h             # C++ helper class
│   └── generictransport.h     # Transport info structure
└── Source/                     # Plugin source code
    ├── PluginProcessor.h      # FL Studio plugin processor
    ├── PluginProcessor.cpp    # Processor implementation
    ├── PluginEditor.h         # JUCE GUI editor
    ├── PluginEditor.cpp       # Editor implementation
    └── PluginEntry.cpp        # DLL entry point
```

## Build Instructions

### Step 1: Clone and Setup

```bash
# Clone the waveform-display repository
git clone https://github.com/yourusername/waveform-display.git
cd waveform-display/FLStudioNative_POC
```

### Step 2: Get JUCE

**Option A: JUCE as Submodule (Recommended)**
```bash
# From repository root
cd ..
git submodule add https://github.com/juce-framework/JUCE.git JUCE
git submodule update --init --recursive
cd FLStudioNative_POC
```

**Option B: System-wide JUCE Installation**
- Download JUCE from [juce.com](https://juce.com/get-juce/)
- Extract to a known location (e.g., `C:/SDKs/JUCE`)
- Update `CMakeLists.txt` to point to this location

### Step 3: Configure CMake

```bash
# Create build directory
mkdir build
cd build

# Configure (adjust JUCE path as needed)
cmake .. -G "Visual Studio 17 2022" -A x64

# Or if JUCE is in a specific location:
cmake .. -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR=C:/SDKs/JUCE
```

### Step 4: Build

**Using Visual Studio:**
```bash
# Open the generated solution
start FLStudioNative_POC.sln

# Build in Visual Studio (Ctrl+Shift+B)
# Or use MSBuild from command line:
msbuild FLStudioNative_POC.sln /p:Configuration=Release
```

**Using CMake:**
```bash
cmake --build . --config Release
```

### Step 5: Install to FL Studio

Copy the built DLL to FL Studio's plugin directory:

```bash
# Default FL Studio plugin path
copy Release\FLStudioNative_POC.dll "C:\Program Files\Image-Line\FL Studio\Plugins\Fruity\"

# Or your custom FL Studio installation path
```

**Note:** You may need Administrator privileges to copy to Program Files.

## Testing the Plugin

### Testing Checklist

After installing the DLL, launch FL Studio and test the following:

- [ ] **Plugin Loads** - Plugin appears in FL Studio's plugin list
- [ ] **Editor Opens** - Clicking the plugin opens the editor window
- [ ] **Timer Updates** - Timer label increments every second
- [ ] **Button Response** - Clicking "Click Me!" button updates status
- [ ] **Slider Works** - Moving gain slider:
  - Updates the value label
  - Updates status message
  - Changes audio gain (if audio is playing through it)
- [ ] **File Chooser** - "Open File Dialog" button opens file browser
  - Selecting a file shows filename in status
  - Canceling shows "cancelled" message
- [ ] **Audio Processing** - Audio plays through the plugin with gain applied
- [ ] **Editor Closes** - Closing plugin window works without crashes
- [ ] **Plugin Reload** - Can close and reopen plugin without issues

### How to Test in FL Studio

1. **Add Plugin to Mixer:**
   - Open FL Studio
   - Open the Mixer (F9)
   - Click an empty mixer slot
   - Select "Fruity" → "FLStudioNative_POC"

2. **Open Editor:**
   - Click the plugin slot to open editor
   - You should see the JUCE GUI with title, timer, slider, and buttons

3. **Test Timer:**
   - Watch the "Timer: 0s" label
   - It should increment every second (1s, 2s, 3s...)

4. **Test Button:**
   - Click "Click Me!" button
   - Status should update with click time

5. **Test Slider:**
   - Move the gain slider
   - Value label should update
   - Status should show new gain value

6. **Test File Chooser:**
   - Click "Open File Dialog"
   - Select any file or cancel
   - Status should reflect your choice

7. **Test Audio:**
   - Play some audio through the mixer channel
   - Adjust gain slider - volume should change
   - Set gain to 0.0 - audio should be silent

## Code Structure Explained

### PluginEntry.cpp
Entry point for FL Studio. Exports `CreatePlugInstance()` function that:
- Initializes JUCE on first load
- Creates processor instances

### PluginProcessor.h/cpp
FL Studio plugin implementation:
- Inherits from `TCPPFruityPlug` (FL Studio SDK)
- Handles `Dispatcher()` for show/hide editor
- Implements `Idle()` to pump JUCE message loop
- Bridges parameters between FL Studio (0-65535 int) and JUCE (0.0-1.0 float)
- Processes audio with simple gain effect

**Key Methods:**
- `createEditorWindow()` - Creates Win32 container and JUCE editor
- `Idle()` - **CRITICAL** - Pumps JUCE MessageManager
- `ProcessParam()` - Handles parameter get/set/hint
- `Eff_Render()` - Processes audio

### PluginEditor.h/cpp
JUCE GUI component:
- Standard JUCE Component with Timer
- Demonstrates various JUCE GUI features
- Updates every second via `timerCallback()`
- Handles button clicks and slider changes
- Shows file chooser dialog

## Troubleshooting

### Plugin doesn't appear in FL Studio
- Check DLL is in correct directory: `FL Studio/Plugins/Fruity/`
- Verify DLL is 64-bit (FL Studio 64-bit requires 64-bit plugins)
- Check Windows Event Viewer for DLL load errors

### Editor window is blank
- Verify JUCE is properly linked
- Check that `Idle()` is being called (add debug logging)
- Ensure MessageManager is initialized

### Timer doesn't update
- `Idle()` not being called by FL Studio
- MessageManager not pumping messages
- Check that `runDispatchLoopUntil(1)` is executing

### Crashes on load
- Check JUCE initialization in `CreatePlugInstance()`
- Verify all dependencies are present
- Use Debug build with Visual Studio debugger attached

### File chooser doesn't work
- Modal dialogs require proper message loop - check `Idle()`
- May need `addToDesktop()` with correct flags

### Build Errors

**JUCE headers not found:**
- Update include paths in CMakeLists.txt
- Set `JUCE_DIR` or `JUCE_MODULES_DIR` in CMake

**Linker errors:**
- Ensure all JUCE modules are linked
- Check that PluginExport.def is included in build

**Export function not found:**
- Verify PluginExport.def is correct
- Check that `/DEF:` flag is set in linker options

## Next Steps: Converting waveform-display Plugin

To convert the full waveform-display plugin to FL Studio native format:

1. **Copy this POC structure** as a template

2. **Replace PluginEditor** with waveform-display editor:
   - Keep the `addToDesktop()` integration
   - Preserve JUCE component hierarchy
   - Maintain timer-based updates

3. **Extend PluginProcessor**:
   - Add all waveform-display parameters
   - Implement proper save/restore for sample data
   - Port audio processing from JUCE AudioProcessor

4. **Parameter Mapping**:
   - Create parameter bridge for all controls
   - Map FL Studio automation to JUCE parameters
   - Handle parameter names and hints

5. **MIDI Integration**:
   - Implement `MIDIIn()` and `MIDITick()`
   - Port JUCE MIDI handling to FL Studio format

6. **State Management**:
   - Implement `SaveRestoreState()` properly
   - Store plugin state in FL Studio format
   - Handle sample references

7. **Testing**:
   - Verify all UI interactions work
   - Test audio processing quality
   - Ensure stability with project save/load

## Additional Resources

- [FL Studio SDK Reference](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/plugins_plugins.htm)
- [JUCE Documentation](https://docs.juce.com/)
- [JUCE Forum](https://forum.juce.com/)
- [Barsay FL Studio SDK CMake Template](https://github.com/Barsay/FL-Studio-SDK-CMake-Template)

## License

This proof-of-concept is provided as-is for educational purposes. Check the main waveform-display repository for license information.

## Notes

- **Windows Only**: FL Studio native plugins are Windows-only
- **64-bit Required**: Modern FL Studio versions require 64-bit plugins
- **No VST3/AU**: This is a native FL plugin, not VST3/AU format
- **Message Loop Critical**: The `Idle()` method is crucial for JUCE GUI
- **Parameter Range**: FL Studio uses 0-65535, JUCE uses 0.0-1.0

## Support

For issues with this POC or questions about FL Studio native plugin development, please open an issue in the waveform-display repository.

---

**Happy Plugin Development! 🎵**
