# FL Studio Native Plugin POC - Quick Start Guide

## For Developers in a Hurry 🚀

This is a working proof-of-concept that shows how to embed JUCE GUI in FL Studio plugins.

### What You Get

- ✅ Complete FL Studio native plugin template
- ✅ JUCE GUI integration (timers, buttons, sliders, file dialogs)
- ✅ Parameter bridging between FL Studio and JUCE
- ✅ Basic audio processing (gain effect)
- ✅ ~1000 lines of well-documented code

### 3-Minute Overview

**The Problem:** FL Studio uses a proprietary plugin API, making it hard to use JUCE.

**The Solution:** Hybrid architecture:
1. FL Studio SDK handles plugin interface
2. Win32 window acts as container
3. JUCE GUI embeds in Win32 window via `addToDesktop()`
4. JUCE message loop pumped via FL Studio's `Idle()` callback

### Files You Need to Understand

| File | What It Does |
|------|--------------|
| `PluginProcessor.cpp` | FL Studio plugin interface + Win32 window creation |
| `PluginEditor.cpp` | Standard JUCE GUI (nothing FL-specific here!) |
| `PluginEntry.cpp` | DLL entry point, JUCE initialization |
| `SDK/*.h` | Minimal FL Studio SDK headers |

### Build in 3 Steps

```bash
# 1. Get JUCE (if you don't have it)
git submodule add https://github.com/juce-framework/JUCE.git ../JUCE

# 2. Configure
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64

# 3. Build
cmake --build . --config Release
```

### Install

```bash
copy Release\FLStudioNative_POC.dll "C:\Program Files\Image-Line\FL Studio\Plugins\Fruity\"
```

### Test

1. Open FL Studio
2. Add plugin to mixer slot
3. Click to open editor
4. Watch timer increment
5. Click buttons, move slider
6. Try file chooser dialog

### Key Code Snippets

**Message Loop (Critical!):**
```cpp
void Idle() {
    // Pump JUCE messages - without this, GUI won't update!
    juce::MessageManager::getInstance()->runDispatchLoopUntil(1);
}
```

**Window Embedding:**
```cpp
// Create Win32 container
containerWindow = CreateWindowEx(..., parentWindow, ...);

// Embed JUCE editor
juceEditor->addToDesktop(0, containerWindow);
```

**Parameter Bridge:**
```cpp
// FL Studio: 0-65535 int → JUCE: 0.0-1.0 float
float flParamToNormalized(int flParam) {
    return static_cast<float>(flParam) / 65535.0f;
}
```

### Common Issues

| Problem | Solution |
|---------|----------|
| Plugin doesn't load | Check 64-bit build, correct directory |
| Blank editor | Verify `Idle()` is calling message pump |
| Timer doesn't update | Check JUCE initialization in `CreatePlugInstance()` |
| Build errors | Update JUCE paths in CMakeLists.txt |

### What to Do Next

**To extend this POC:**
- Add more parameters in `ProcessParam()`
- Enhance GUI in `PluginEditor.cpp`
- Improve audio processing in `Eff_Render()`
- Implement save/load in `SaveRestoreState()`

**To convert your JUCE plugin:**
1. Copy this POC structure
2. Replace `HelloWorldEditor` with your editor
3. Port audio processing to `Eff_Render()`
4. Map all parameters
5. Test in FL Studio

### Documentation

- **README.md** - Full documentation with troubleshooting
- **IMPLEMENTATION_NOTES.md** - Technical deep dive
- **VERIFICATION_CHECKLIST.md** - Requirements verification

### Requirements

- Windows 10/11
- Visual Studio 2019/2022
- CMake 3.22+
- JUCE 7.x+
- FL Studio 20.8+ (for testing)

### License

See main repository for license information.

---

**Need help?** Check the full README.md or open an issue!

**Ready to build?** See detailed build instructions in README.md!
