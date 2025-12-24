# Visual Guide - Stage 1 Expected Output

## What You Should See

When you run `JustASample_Win32.exe` from Stage 1, you should see this:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Just a Sample - Win32 Version                                    ▯ ▢ ✕     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌───────────────────────────────────────────────────────────────────────┐ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                     Just a Sample - Win32 Version                    │ │
│  │                                                                       │ │
│  │                 Stage 1: Basic Window (COMPLETE)                     │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  │                                                                       │ │
│  └───────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Window Details

### Title Bar
- **Title:** "Just a Sample - Win32 Version"
- **Buttons:** Minimize, Maximize, Close (standard Windows)

### Main Window
- **Size:** 900 pixels wide × 600 pixels tall (approximately)
- **Background:** Light gray (#F0F0F5)
- **Border:** Dark gray line, 2 pixels, 10 pixels from edges

### Text Display
- **Line 1:** "Just a Sample - Win32 Version"
- **Line 2:** "Stage 1: Basic Window (COMPLETE)"
- **Position:** Centered in window
- **Font:** Default Windows font (Segoe UI)

### Colors (RGB)
- Background: (240, 240, 245) - Light grayish blue
- Border: (100, 100, 120) - Dark gray
- Text: Black (default)

## Testing Interactions

### 1. Window Controls

**Minimize Button:**
```
Click [▯] → Window minimizes to taskbar
```

**Maximize Button:**
```
Click [▢] → Window fills screen
Click again → Window returns to original size
```

**Close Button:**
```
Click [✕] → Confirmation dialog appears
```

### 2. Close Confirmation Dialog

When you click the close button:

```
┌─────────────────────────────┐
│ Confirm                ─ ✕ │
├─────────────────────────────┤
│  ⚠                          │
│     Are you sure you want   │
│     to close?               │
│                             │
│         [ Yes ]  [ No ]     │
└─────────────────────────────┘
```

- **Click "Yes"**: Window closes
- **Click "No"**: Dialog closes, window stays open
- **Click [✕]**: Dialog closes, window stays open

### 3. Window Resize

```
Drag any edge or corner → Window resizes
                        → Content repaints
                        → Text remains centered
```

## Expected Behavior Checklist

Before moving to Stage 2, verify:

### Window Creation
- [ ] Window appears on screen
- [ ] Window has correct title
- [ ] Window is approximately 900×600 pixels
- [ ] Window appears centered on screen (or in default position)

### Visual Elements
- [ ] Background is light gray/blue
- [ ] Border is visible (dark gray, 2px thick, 10px from edges)
- [ ] Text is visible and centered
- [ ] Text shows both lines correctly

### Window Controls
- [ ] Minimize button works (window hides to taskbar)
- [ ] Maximize button works (window fills screen)
- [ ] Restore from maximize works (window returns to original size)
- [ ] Window can be moved by dragging title bar
- [ ] Window can be resized by dragging edges/corners

### Close Behavior
- [ ] Close button shows confirmation dialog
- [ ] Dialog has "Yes" and "No" buttons
- [ ] "Yes" closes the window
- [ ] "No" keeps window open
- [ ] Dialog's [✕] button keeps window open

### Resize Behavior
- [ ] Window can be resized smaller
- [ ] Window can be resized larger
- [ ] Content repaints when resized
- [ ] Text remains visible after resize
- [ ] No flickering or artifacts

### Stability
- [ ] No crashes on startup
- [ ] No crashes during use
- [ ] No crashes on close
- [ ] No error messages or warnings
- [ ] No hanging or freezing

## Common Issues and Solutions

### Issue: Window appears but is blank
**Expected:** Light gray background with border and text  
**If blank:** Check that WM_PAINT message is being processed

### Issue: Text is not visible
**Expected:** Two lines of centered text  
**If missing:** Check that WM_CREATE is creating the static control

### Issue: Close button closes immediately
**Expected:** Confirmation dialog appears first  
**If immediate:** Check WM_CLOSE handler

### Issue: Window is wrong size
**Expected:** Approximately 900×600 client area  
**If wrong:** Check WINDOW_WIDTH and WINDOW_HEIGHT constants

### Issue: Colors are different
**Expected:** Light grayish-blue background  
**If different:** Check RGB(240, 240, 245) in WM_PAINT

## Screenshot Comparison

If you take a screenshot of Stage 1, you should see:

**Top Section:**
- Standard Windows title bar
- Title: "Just a Sample - Win32 Version"
- Standard minimize/maximize/close buttons

**Middle Section:**
- Large light gray area
- Thin dark border
- Centered text showing:
  - "Just a Sample - Win32 Version"
  - "Stage 1: Basic Window (COMPLETE)"

**Bottom Section:**
- Continuation of light gray area
- Border continues to bottom

## Comparison with Original Plugin

**Original JUCE Plugin:**
- Complex UI with multiple controls
- Waveform display area
- Rotary knobs and sliders
- Effects chain section
- Menu bar and status area

**Stage 1 Win32 Version:**
- Simple window (foundation only)
- Basic text display
- No controls yet
- No waveform display yet
- No effects yet

**This is expected!** We're building the foundation first. Each subsequent stage will add more features until we match the original plugin.

## Next Stage Preview

**Stage 2 will add:**
- Custom-drawn controls (buttons)
- Plugin-style layout
- More sophisticated graphics
- Basic control interaction

But first, **Stage 1 must work perfectly!**

## Success Criteria

✅ **Stage 1 is successful when:**
1. Application builds without errors
2. Application runs without crashes
3. Window appears as shown in this guide
4. All interactions work as described
5. All checklist items are verified

✅ **When all above are confirmed, you're ready for Stage 2!**

---

**Reference this guide while testing Stage 1. Good luck! 🎯**
