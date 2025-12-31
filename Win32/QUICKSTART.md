# Quick Start Guide - Win32 Version

## Building the Application

### Prerequisites
- Windows 10 or later
- Visual Studio 2019 or later with C++ development tools
- CMake 3.15 or later (optional, can also use Visual Studio's built-in CMake support)

### Method 1: Using Visual Studio (Recommended)

1. Open Visual Studio 2019 or later
2. Select "Open a local folder"
3. Navigate to and select the repository root folder
4. Visual Studio will automatically detect `CMakeLists.txt`
5. Wait for CMake configuration to complete
6. Select "Build > Build All" or press Ctrl+Shift+B
7. The executable will be in `build/bin/Debug/JustASampleWin32.exe`

### Method 2: Using CMake Command Line

```cmd
cd /path/to/waveform-display
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable will be in `build/bin/Release/JustASampleWin32.exe`

### Method 3: Generate Visual Studio Solution

```cmd
cd /path/to/waveform-display
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
```

Then open `JustASampleWin32.sln` in Visual Studio.

## Running the Application

### First Run

1. Launch `JustASampleWin32.exe`
2. The main window will appear with basic controls
3. Audio system initializes automatically using default output device

### Basic Usage

**Loading a Sample:**
- Click "Load Sample" button (currently shows placeholder message)
- Full file dialog integration pending

**Playing Audio:**
- Click "Play" button to trigger a test note (A5)
- MIDI keyboard input supported if device connected

**Adjusting Gain:**
- Drag the gain rotary control to adjust volume
- Values range from 0.0 (silence) to 1.0 (full volume)

## Current Features

### Working
✓ Double-buffered window rendering (no flicker)
✓ WASAPI audio output initialization
✓ MIDI input handling
✓ Basic UI components (buttons, labels, sliders)
✓ Parameter control
✓ Simple test note playback

### In Progress
⧗ File loading functionality
⧗ Waveform visualization
⧗ Complete UI layout
⧗ Effects visualization

### Not Yet Implemented
✗ Audio file format support (WAV, MP3, etc.)
✗ Time stretching
✗ Recording
✗ State save/load

## Troubleshooting

### Audio Issues

**No sound when clicking Play:**
1. Check Windows audio settings
2. Ensure default playback device is set
3. Check volume mixer for the application
4. Verify WASAPI initialization in debug output

**MIDI not responding:**
1. Connect MIDI device before launching application
2. Check MIDI device is recognized in Windows
3. Only first MIDI input device is currently used

### Build Issues

**Cannot find Windows SDK:**
- Install Windows 10 SDK via Visual Studio Installer
- Verify SDK path in Visual Studio settings

**Link errors for winmm.lib, ole32.lib, etc:**
- Ensure CMake configuration completed successfully
- These libraries are standard with Visual Studio

**CMake configuration fails:**
- Update CMake to latest version
- Ensure Visual Studio C++ tools are installed
- Check CMakeLists.txt is in repository root

### Visual Issues

**Window appears blank:**
- Check graphics drivers are up to date
- Verify double buffering initialization
- Look for errors in debug output

**Flickering or tearing:**
- Should not occur - this indicates double buffering issue
- Check WM_ERASEBKGND returns 1
- Verify BitBlt is used for final screen update

## Architecture Overview

### Audio Thread
- Runs independently from GUI
- WASAPI callback-based rendering
- Processes MIDI events
- Renders synthesizer voices
- Applies effects chain

### GUI Thread
- Handles all Windows messages
- Processes mouse/keyboard input
- Renders UI components
- Updates visual parameters

### Thread Safety
- Critical sections protect shared audio data
- Atomic operations for simple state flags
- GUI and audio threads are properly isolated

## Development Notes

### Adding New Components
1. Create component class derived from `Component`
2. Override `paint()` for custom rendering
3. Add mouse event handlers if interactive
4. Add to component hierarchy in `CreateUI()`

### Modifying Audio Processing
1. Edit `SamplerProcessor::processBlock()`
2. Add parameters to `Parameters` enum
3. Implement getter/setter methods
4. Wire to UI controls

### Debugging Tips
- Use Debug build for detailed output
- Set breakpoints in WndProc for event debugging
- Check audio thread with thread debugging
- Use Performance Profiler for optimization

## Performance Expectations

### CPU Usage
- Idle: <1% CPU
- Playing single note: 1-3% CPU
- Complex polyphony + effects: 5-15% CPU

### Memory
- Base application: ~10-20 MB
- With sample loaded: Depends on sample size
- No memory leaks should occur

### Latency
- WASAPI shared mode: ~10-30ms typical
- MIDI to audio: <5ms processing overhead
- UI responsiveness: <16ms (60 FPS target)

## Next Steps

To continue development:

1. **Implement file I/O**: Add audio file reading support
2. **Complete UI**: Implement remaining components from original JUCE version
3. **Add effects**: Finish reverb, chorus, EQ implementations
4. **Time stretching**: Integrate Bungee library
5. **State management**: Add save/load functionality

See `Win32/README.md` for detailed architecture documentation.
See `Win32/IMPLEMENTATION_NOTES.md` for technical implementation details.

## Support

For issues specific to this Win32 conversion, please check:
- IMPLEMENTATION_NOTES.md for technical details
- README.md in Win32 folder for architecture info
- Original JUCE project for feature reference

## License

Same as original Just a Sample project.
