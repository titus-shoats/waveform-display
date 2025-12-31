# Just a Sample - Win32 API Version

This is a native Win32 API conversion of the JUCE-based "Just a Sample" audio sampler plugin.

## Overview

This conversion replaces the JUCE framework with native Win32 APIs while maintaining the core functionality of the original audio sampler. The implementation follows Win32 best practices for performance and stability.

## Architecture

### Audio System
- **WASAPI** (Windows Audio Session API) for low-latency audio playback
- **Win32 MIDI APIs** for MIDI input handling
- Custom audio buffer management and DSP processing
- Multi-threaded audio rendering for real-time performance

### GUI System
- **Win32 GDI** for graphics rendering
- **Double buffering** to eliminate flickering and visual artifacts
- Component-based architecture similar to JUCE
- Custom controls: rotary sliders, buttons, waveform display

### Best Practices Implemented

#### 1. Window Class Registration (No CS_HREDRAW/CS_VREDRAW)
```cpp
wc.style = 0;  // No CS_HREDRAW/CS_VREDRAW
```
This prevents automatic full window redraws on resize events, improving performance.

#### 2. Window Creation (WS_CLIPCHILDREN)
```cpp
CreateWindowEx(..., WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, ...)
```
WS_CLIPCHILDREN prevents the parent from painting over child windows, essential for proper rendering of child controls.

#### 3. Double Buffering
All drawing operations use memory DCs to render off-screen first, then blit to the screen:
```cpp
HDC memDC = CreateCompatibleDC(hdc);
HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
// Draw to memDC...
BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
```

#### 4. WM_ERASEBKGND Handling
```cpp
case WM_ERASEBKGND:
    return 1;  // Don't erase - we handle it in double buffered paint
```
This prevents flickering by not erasing the background separately from painting.

## Project Structure

```
Win32/
├── main.cpp                  - Application entry point and window procedure
├── AudioProcessor.h/cpp      - Base audio processing classes
├── SamplerProcessor.h/cpp    - Sampler-specific audio processing
├── GUIComponents.h/cpp       - GUI component system
└── (Additional files to be added)
```

## Building

### Requirements
- Windows 10 or later
- Visual Studio 2019 or later (with C++17 support)
- CMake 3.15 or later

### Build Instructions

#### Using CMake:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

#### Using Visual Studio:
1. Open CMakeLists.txt with Visual Studio
2. Select Release configuration
3. Build > Build Solution

## Features Converted

### Core Audio Features
- [x] WASAPI audio output
- [x] Win32 MIDI input
- [x] Basic sampler voice synthesis
- [x] Audio buffer management
- [ ] File loading (WAV, MP3, FLAC, etc.) - Requires additional codec libraries
- [ ] Time stretching (Bungee integration)
- [ ] Pitch detection

### GUI Features
- [x] Main window with double buffering
- [x] Component-based architecture
- [x] Basic controls (buttons, labels, sliders)
- [x] Waveform display component
- [ ] Full UI layout matching original
- [ ] FX chain visualization
- [ ] Sample navigator
- [ ] Recording interface

### Effects
- [x] Basic reverb (delay-based)
- [x] Simple distortion (waveshaping)
- [ ] Chorus
- [ ] Band EQ with proper filters
- [ ] Pre/post FX routing

## Differences from JUCE Version

### What's Different
1. **No plugin format support** - This is a standalone Win32 application, not a VST3/AU plugin
2. **Windows-only** - Native Win32 APIs are Windows-specific
3. **Different file I/O** - Uses Win32 file APIs instead of JUCE's cross-platform abstraction
4. **Simplified effects** - Some effects are simplified due to removal of external DSP libraries
5. **No cross-platform GUI** - Uses Win32 GDI instead of JUCE's graphics abstraction

### What's Preserved
1. **Core sampler functionality** - Sample playback, MIDI triggering, pitch shifting
2. **Parameter system** - Similar parameter management
3. **Component hierarchy** - Similar GUI component organization
4. **Audio processing flow** - Same basic audio processing architecture

## Performance Optimizations

### Rendering
- Double buffering eliminates visual tearing and flickering
- Selective invalidation minimizes unnecessary redraws
- No automatic full window redraws on resize

### Audio
- Multi-threaded audio rendering
- Lock-free atomic operations where possible
- Efficient buffer management

## Known Limitations

1. **Audio file formats**: Currently only basic WAV support planned. MP3, FLAC, etc. require additional libraries (e.g., libsndfile, dr_libs)
2. **Plugin hosting**: Not designed as a plugin, only standalone application
3. **Cross-platform**: Windows-only implementation
4. **Time stretching**: Bungee library integration pending
5. **Advanced DSP**: Some JUCE DSP features simplified or pending implementation

## Future Work

### High Priority
- [ ] Implement proper WAV file loading
- [ ] Add file drag-and-drop support
- [ ] Complete UI layout implementation
- [ ] Integrate Bungee time stretcher
- [ ] Add proper EQ filters (biquad implementation)

### Medium Priority
- [ ] State save/load functionality
- [ ] Recording from audio input
- [ ] MIDI device selection UI
- [ ] Audio device selection UI
- [ ] Preset management

### Low Priority
- [ ] OpenGL rendering for better performance
- [ ] Additional audio formats
- [ ] VST3 host support (for loading as plugin)
- [ ] Network MIDI support

## Code Organization

### Key Classes

**AudioProcessor** (AudioProcessor.h/cpp)
- Base class for audio processing
- WASAPI audio output management
- MIDI input handling
- Audio thread management

**SamplerProcessor** (SamplerProcessor.h/cpp)
- Derived from AudioProcessor
- Sample playback logic
- Effects processing
- Parameter management

**Component** (GUIComponents.h/cpp)
- Base GUI component class
- Hierarchical component system
- Event handling

**Synthesizer** (AudioProcessor.h/cpp)
- Voice management
- MIDI note handling
- Polyphonic playback

## Technical Notes

### Thread Safety
- Audio processing runs on dedicated thread
- Critical sections protect shared resources
- Atomic operations for simple state flags

### Memory Management
- RAII patterns for resource cleanup
- Smart pointers where appropriate
- Manual resource management for Win32 handles

### Error Handling
- HRESULT checks for all COM operations
- Proper resource cleanup on failure
- Graceful degradation when possible

## Contributing

When contributing to the Win32 version:

1. Follow Win32 best practices:
   - Always clean up GDI resources (pens, brushes, fonts, etc.)
   - Use double buffering for all custom drawing
   - Minimize critical section lock time
   - Check HRESULT for all COM operations

2. Maintain consistency with original JUCE architecture where possible
3. Document Win32-specific patterns and idioms
4. Test for memory leaks with tools like Visual Studio diagnostic tools

## License

Same as original Just a Sample project - see LICENSE file.

## Credits

Original JUCE version by BOBONA: https://github.com/BOBONA/Just-a-Sample

Win32 conversion implements the same functionality using native Windows APIs.
