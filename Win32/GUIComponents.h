/*
  ==============================================================================

    GUIComponents.h
    Win32 GUI Components (replaces JUCE Components)
    
    This provides a component-based GUI system using Win32 API with
    double buffering to prevent flickering.

  ==============================================================================
*/

#pragma once

#include <windows.h>
#include <vector>
#include <memory>
#include <string>
#include <functional>

// Base component class (Win32 equivalent of JUCE Component)
class Component
{
public:
    Component();
    virtual ~Component();

    // Painting
    virtual void paint(HDC hdc);
    virtual void paintChildren(HDC hdc);

    // Layout
    virtual void resized() {}
    void setBounds(int x, int y, int width, int height);
    void setSize(int width, int height);
    void setPosition(int x, int y);
    
    RECT getBounds() const { return m_bounds; }
    int getX() const { return m_bounds.left; }
    int getY() const { return m_bounds.top; }
    int getWidth() const { return m_bounds.right - m_bounds.left; }
    int getHeight() const { return m_bounds.bottom - m_bounds.top; }

    // Hierarchy
    void addChildComponent(Component* child);
    void removeChildComponent(Component* child);
    Component* getParent() const { return m_parent; }
    const std::vector<Component*>& getChildren() const { return m_children; }

    // Visibility
    void setVisible(bool shouldBeVisible);
    bool isVisible() const { return m_visible; }

    // Mouse events
    virtual void mouseDown(int x, int y, UINT button) { (void)x; (void)y; (void)button; }
    virtual void mouseUp(int x, int y, UINT button) { (void)x; (void)y; (void)button; }
    virtual void mouseMove(int x, int y) { (void)x; (void)y; }
    virtual void mouseDrag(int x, int y) { (void)x; (void)y; }
    virtual void mouseWheel(int x, int y, int delta) { (void)x; (void)y; (void)delta; }
    virtual void mouseEnter() {}
    virtual void mouseExit() {}

    // Hit testing
    bool hitTest(int x, int y) const;
    Component* getComponentAt(int x, int y);

    // Invalidation
    void repaint();
    void repaint(const RECT& area);

    // Window handle (for top-level components)
    void setWindowHandle(HWND hwnd) { m_hwnd = hwnd; }
    HWND getWindowHandle() const { return m_hwnd; }

protected:
    RECT m_bounds;
    Component* m_parent;
    std::vector<Component*> m_children;
    bool m_visible;
    HWND m_hwnd;
    
    COLORREF m_backgroundColor;
    COLORREF m_foregroundColor;
};

// Label component
class Label : public Component
{
public:
    Label(const std::wstring& text = L"");
    
    void setText(const std::wstring& text);
    std::wstring getText() const { return m_text; }
    
    void setFont(HFONT font);
    void setTextColor(COLORREF color) { m_textColor = color; }
    void setAlignment(UINT alignment) { m_alignment = alignment; }
    
    void paint(HDC hdc) override;

private:
    std::wstring m_text;
    HFONT m_font;
    COLORREF m_textColor;
    UINT m_alignment;
};

// Button component
class Button : public Component
{
public:
    Button(const std::wstring& text = L"");
    
    void setText(const std::wstring& text);
    std::wstring getText() const { return m_text; }
    
    void setClickCallback(std::function<void()> callback) { m_clickCallback = callback; }
    
    void paint(HDC hdc) override;
    void mouseDown(int x, int y, UINT button) override;
    void mouseUp(int x, int y, UINT button) override;
    void mouseEnter() override;
    void mouseExit() override;

private:
    std::wstring m_text;
    bool m_isPressed;
    bool m_isHovered;
    std::function<void()> m_clickCallback;
};

// Slider component (for rotary controls)
class Slider : public Component
{
public:
    enum Style
    {
        LinearHorizontal,
        LinearVertical,
        Rotary
    };

    Slider(Style style = Rotary);
    
    void setValue(float value);
    float getValue() const { return m_value; }
    
    void setRange(float min, float max);
    void setValueChangedCallback(std::function<void(float)> callback) { m_valueChangedCallback = callback; }
    
    void paint(HDC hdc) override;
    void mouseDown(int x, int y, UINT button) override;
    void mouseUp(int x, int y, UINT button) override;
    void mouseDrag(int x, int y) override;

private:
    Style m_style;
    float m_value;
    float m_minValue;
    float m_maxValue;
    bool m_isDragging;
    int m_dragStartY;
    float m_dragStartValue;
    std::function<void(float)> m_valueChangedCallback;
    
    void paintRotary(HDC hdc);
    void paintLinear(HDC hdc);
};

// Waveform display component
class WaveformDisplay : public Component
{
public:
    WaveformDisplay();
    
    void setSample(const float* sampleData, int numSamples, int numChannels);
    void setViewRange(int startSample, int endSample);
    void setZoom(float zoom);
    
    void paint(HDC hdc) override;
    void mouseDown(int x, int y, UINT button) override;
    void mouseUp(int x, int y, UINT button) override;
    void mouseDrag(int x, int y) override;
    void mouseWheel(int x, int y, int delta) override;

private:
    std::vector<float> m_sampleData;
    int m_numSamples;
    int m_numChannels;
    int m_viewStart;
    int m_viewEnd;
    float m_zoom;
    
    bool m_isDragging;
    int m_dragStartX;
    int m_dragStartSample;
    
    // Double buffering for waveform
    HDC m_waveformDC;
    HBITMAP m_waveformBitmap;
    HBITMAP m_waveformOldBitmap;
    bool m_needsRedraw;
    
    void createWaveformBuffer();
    void destroyWaveformBuffer();
    void renderWaveform();
};

// Component manager for handling mouse events and rendering
class ComponentManager
{
public:
    ComponentManager(HWND hwnd);
    ~ComponentManager();

    void setRootComponent(Component* root);
    Component* getRootComponent() const { return m_rootComponent; }

    // Event handling
    void handleMouseMove(int x, int y);
    void handleMouseDown(int x, int y, UINT button);
    void handleMouseUp(int x, int y, UINT button);
    void handleMouseWheel(int x, int y, int delta);

    // Rendering
    void render(HDC hdc);

private:
    HWND m_hwnd;
    Component* m_rootComponent;
    Component* m_hoveredComponent;
    Component* m_draggedComponent;
    
    Component* findComponentAt(int x, int y);
};

// Double-buffered painter helper
class DoubleBufferedPainter
{
public:
    DoubleBufferedPainter();
    ~DoubleBufferedPainter();

    bool initialize(HWND hwnd, int width, int height);
    void cleanup();
    
    HDC getMemoryDC() const { return m_memDC; }
    void blitToScreen(HDC screenDC, int width, int height);

private:
    HDC m_memDC;
    HBITMAP m_memBitmap;
    HBITMAP m_oldBitmap;
    int m_width;
    int m_height;
};
