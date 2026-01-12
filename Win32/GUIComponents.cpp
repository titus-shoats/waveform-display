/*
  ==============================================================================

    GUIComponents.cpp
    Win32 GUI Components Implementation

  ==============================================================================
*/

#include "GUIComponents.h"
#include <algorithm>
#include <cmath>

// Component implementation
Component::Component()
    : m_parent(nullptr)
    , m_visible(true)
    , m_hwnd(nullptr)
    , m_backgroundColor(RGB(255, 255, 255))
    , m_foregroundColor(RGB(0, 0, 0))
{
    m_bounds = {0, 0, 0, 0};
}

Component::~Component()
{
    for (auto* child : m_children)
    {
        delete child;
    }
    m_children.clear();
}

void Component::paint(HDC hdc)
{
    if (!m_visible)
        return;

    // Default implementation - fill with background color
    HBRUSH brush = CreateSolidBrush(m_backgroundColor);
    FillRect(hdc, &m_bounds, brush);
    DeleteObject(brush);
}

void Component::paintChildren(HDC hdc)
{
    for (auto* child : m_children)
    {
        if (child->isVisible())
        {
            child->paint(hdc);
            child->paintChildren(hdc);
        }
    }
}

void Component::setBounds(int x, int y, int width, int height)
{
    m_bounds.left = x;
    m_bounds.top = y;
    m_bounds.right = x + width;
    m_bounds.bottom = y + height;
    resized();
}

void Component::setSize(int width, int height)
{
    m_bounds.right = m_bounds.left + width;
    m_bounds.bottom = m_bounds.top + height;
    resized();
}

void Component::setPosition(int x, int y)
{
    int width = m_bounds.right - m_bounds.left;
    int height = m_bounds.bottom - m_bounds.top;
    m_bounds.left = x;
    m_bounds.top = y;
    m_bounds.right = x + width;
    m_bounds.bottom = y + height;
}

void Component::addChildComponent(Component* child)
{
    if (child)
    {
        m_children.push_back(child);
        child->m_parent = this;
    }
}

void Component::removeChildComponent(Component* child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end())
    {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
    }
}

void Component::setVisible(bool shouldBeVisible)
{
    m_visible = shouldBeVisible;
    repaint();
}

bool Component::hitTest(int x, int y) const
{
    return x >= m_bounds.left && x < m_bounds.right &&
           y >= m_bounds.top && y < m_bounds.bottom;
}

Component* Component::getComponentAt(int x, int y)
{
    // Check children first (reverse order for top-down hit testing)
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        Component* child = *it;
        if (child->isVisible() && child->hitTest(x, y))
        {
            Component* found = child->getComponentAt(x, y);
            if (found)
                return found;
        }
    }
    
    // Check this component
    if (hitTest(x, y))
        return this;
        
    return nullptr;
}

void Component::repaint()
{
    if (m_hwnd)
    {
        InvalidateRect(m_hwnd, &m_bounds, FALSE);
    }
    else if (m_parent)
    {
        m_parent->repaint();
    }
}

void Component::repaint(const RECT& area)
{
    if (m_hwnd)
    {
        InvalidateRect(m_hwnd, &area, FALSE);
    }
    else if (m_parent)
    {
        m_parent->repaint(area);
    }
}

// Label implementation
Label::Label(const std::wstring& text)
    : m_text(text)
    , m_font(nullptr)
    , m_textColor(RGB(0, 0, 0))
    , m_alignment(DT_LEFT | DT_VCENTER | DT_SINGLELINE)
{
    // Create default font
    m_font = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void Label::setText(const std::wstring& text)
{
    if (m_text != text)
    {
        m_text = text;
        repaint();
    }
}

void Label::setFont(HFONT font)
{
    if (m_font && m_font != font)
    {
        DeleteObject(m_font);
    }
    m_font = font;
    repaint();
}

void Label::paint(HDC hdc)
{
    Component::paint(hdc);
    
    if (m_text.empty())
        return;

    HFONT oldFont = (HFONT)SelectObject(hdc, m_font);
    int oldMode = SetBkMode(hdc, TRANSPARENT);
    COLORREF oldColor = SetTextColor(hdc, m_textColor);
    
    DrawText(hdc, m_text.c_str(), -1, &m_bounds, m_alignment);
    
    SetTextColor(hdc, oldColor);
    SetBkMode(hdc, oldMode);
    SelectObject(hdc, oldFont);
}

// Button implementation
Button::Button(const std::wstring& text)
    : m_text(text)
    , m_isPressed(false)
    , m_isHovered(false)
{
}

void Button::setText(const std::wstring& text)
{
    if (m_text != text)
    {
        m_text = text;
        repaint();
    }
}

void Button::paint(HDC hdc)
{
    // Draw button background
    COLORREF bgColor = m_isPressed ? RGB(180, 180, 180) :
                      m_isHovered ? RGB(220, 220, 220) :
                      RGB(240, 240, 240);
    
    HBRUSH brush = CreateSolidBrush(bgColor);
    FillRect(hdc, &m_bounds, brush);
    DeleteObject(brush);
    
    // Draw border
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    
    MoveToEx(hdc, m_bounds.left, m_bounds.top, nullptr);
    LineTo(hdc, m_bounds.right - 1, m_bounds.top);
    LineTo(hdc, m_bounds.right - 1, m_bounds.bottom - 1);
    LineTo(hdc, m_bounds.left, m_bounds.bottom - 1);
    LineTo(hdc, m_bounds.left, m_bounds.top);
    
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    
    // Draw text
    if (!m_text.empty())
    {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawText(hdc, m_text.c_str(), -1, &m_bounds, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void Button::mouseDown(int x, int y, UINT button)
{
    m_isPressed = true;
    repaint();
}

void Button::mouseUp(int x, int y, UINT button)
{
    if (m_isPressed && hitTest(x, y))
    {
        if (m_clickCallback)
            m_clickCallback();
    }
    m_isPressed = false;
    repaint();
}

void Button::mouseEnter()
{
    m_isHovered = true;
    repaint();
}

void Button::mouseExit()
{
    m_isHovered = false;
    m_isPressed = false;
    repaint();
}

// Slider implementation
Slider::Slider(Style style)
    : m_style(style)
    , m_value(0.5f)
    , m_minValue(0.0f)
    , m_maxValue(1.0f)
    , m_isDragging(false)
    , m_dragStartY(0)
    , m_dragStartValue(0.0f)
{
}

void Slider::setValue(float value)
{
    float newValue = std::max(m_minValue, std::min(m_maxValue, value));
    if (m_value != newValue)
    {
        m_value = newValue;
        repaint();
    }
}

void Slider::setRange(float min, float max)
{
    m_minValue = min;
    m_maxValue = max;
    setValue(m_value); // Clamp to new range
}

void Slider::paint(HDC hdc)
{
    if (m_style == Rotary)
        paintRotary(hdc);
    else
        paintLinear(hdc);
}

void Slider::paintRotary(HDC hdc)
{
    int centerX = (m_bounds.left + m_bounds.right) / 2;
    int centerY = (m_bounds.top + m_bounds.bottom) / 2;
    int radius = std::min(m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top) / 2 - 4;
    
    // Draw outer circle
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH brush = CreateSolidBrush(RGB(240, 240, 240));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    
    Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
    
    // Draw value indicator
    float normalizedValue = (m_value - m_minValue) / (m_maxValue - m_minValue);
    float angle = -2.35619f + normalizedValue * 4.71239f; // -135° to +135° in radians
    
    int indicatorX = centerX + static_cast<int>(radius * 0.7f * std::cos(angle));
    int indicatorY = centerY + static_cast<int>(radius * 0.7f * std::sin(angle));
    
    HPEN valuePen = CreatePen(PS_SOLID, 3, RGB(0, 120, 215));
    SelectObject(hdc, valuePen);
    
    MoveToEx(hdc, centerX, centerY, nullptr);
    LineTo(hdc, indicatorX, indicatorY);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
    DeleteObject(valuePen);
}

void Slider::paintLinear(HDC hdc)
{
    // Draw track
    HBRUSH trackBrush = CreateSolidBrush(RGB(200, 200, 200));
    
    RECT trackRect = m_bounds;
    if (m_style == LinearHorizontal)
    {
        trackRect.top = (m_bounds.top + m_bounds.bottom) / 2 - 2;
        trackRect.bottom = trackRect.top + 4;
    }
    else
    {
        trackRect.left = (m_bounds.left + m_bounds.right) / 2 - 2;
        trackRect.right = trackRect.left + 4;
    }
    
    FillRect(hdc, &trackRect, trackBrush);
    DeleteObject(trackBrush);
    
    // Draw thumb
    float normalizedValue = (m_value - m_minValue) / (m_maxValue - m_minValue);
    int thumbX, thumbY;
    
    if (m_style == LinearHorizontal)
    {
        thumbX = m_bounds.left + static_cast<int>(normalizedValue * (m_bounds.right - m_bounds.left));
        thumbY = (m_bounds.top + m_bounds.bottom) / 2;
    }
    else
    {
        thumbX = (m_bounds.left + m_bounds.right) / 2;
        thumbY = m_bounds.bottom - static_cast<int>(normalizedValue * (m_bounds.bottom - m_bounds.top));
    }
    
    HBRUSH thumbBrush = CreateSolidBrush(RGB(0, 120, 215));
    HPEN thumbPen = CreatePen(PS_SOLID, 1, RGB(0, 100, 180));
    HPEN oldPen = (HPEN)SelectObject(hdc, thumbPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, thumbBrush);
    
    const int thumbRadius = 8;
    Ellipse(hdc, thumbX - thumbRadius, thumbY - thumbRadius,
            thumbX + thumbRadius, thumbY + thumbRadius);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(thumbBrush);
    DeleteObject(thumbPen);
}

void Slider::mouseDown(int x, int y, UINT button)
{
    m_isDragging = true;
    m_dragStartY = y;
    m_dragStartValue = m_value;
}

void Slider::mouseUp(int x, int y, UINT button)
{
    m_isDragging = false;
}

void Slider::mouseDrag(int x, int y)
{
    if (!m_isDragging)
        return;
        
    float deltaY = static_cast<float>(m_dragStartY - y);
    float sensitivity = 0.005f;
    float newValue = m_dragStartValue + deltaY * sensitivity * (m_maxValue - m_minValue);
    
    setValue(newValue);
    
    if (m_valueChangedCallback)
        m_valueChangedCallback(m_value);
}

// WaveformDisplay implementation
WaveformDisplay::WaveformDisplay()
    : m_numSamples(0)
    , m_numChannels(0)
    , m_viewStart(0)
    , m_viewEnd(0)
    , m_zoom(1.0f)
    , m_isDragging(false)
    , m_dragStartX(0)
    , m_dragStartSample(0)
    , m_waveformDC(nullptr)
    , m_waveformBitmap(nullptr)
    , m_waveformOldBitmap(nullptr)
    , m_needsRedraw(true)
{
    m_backgroundColor = RGB(30, 30, 30);
}

void WaveformDisplay::setSample(const float* sampleData, int numSamples, int numChannels)
{
    m_sampleData.assign(sampleData, sampleData + numSamples * numChannels);
    m_numSamples = numSamples;
    m_numChannels = numChannels;
    m_viewStart = 0;
    m_viewEnd = numSamples;
    m_needsRedraw = true;
    repaint();
}

void WaveformDisplay::setViewRange(int startSample, int endSample)
{
    m_viewStart = std::max(0, startSample);
    m_viewEnd = std::min(m_numSamples, endSample);
    m_needsRedraw = true;
    repaint();
}

void WaveformDisplay::setZoom(float zoom)
{
    m_zoom = std::max(0.1f, std::min(100.0f, zoom));
    m_needsRedraw = true;
    repaint();
}

void WaveformDisplay::paint(HDC hdc)
{
    Component::paint(hdc);
    
    if (m_sampleData.empty() || m_numSamples == 0)
        return;

    int width = m_bounds.right - m_bounds.left;
    int height = m_bounds.bottom - m_bounds.top;
    int channelHeight = height / std::max(1, m_numChannels);
    
    // Draw waveform for each channel
    for (int ch = 0; ch < m_numChannels; ++ch)
    {
        int yOffset = m_bounds.top + ch * channelHeight;
        int centerY = yOffset + channelHeight / 2;
        
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 200, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        
        int samplesPerPixel = std::max(1, (m_viewEnd - m_viewStart) / width);
        
        for (int x = 0; x < width; ++x)
        {
            int sampleIndex = m_viewStart + (x * (m_viewEnd - m_viewStart)) / width;
            if (sampleIndex >= m_numSamples)
                break;
                
            float sample = m_sampleData[sampleIndex * m_numChannels + ch];
            int y = centerY - static_cast<int>(sample * channelHeight * 0.45f);
            
            if (x == 0)
                MoveToEx(hdc, m_bounds.left + x, y, nullptr);
            else
                LineTo(hdc, m_bounds.left + x, y);
        }
        
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
}

void WaveformDisplay::mouseDown(int x, int y, UINT button)
{
    m_isDragging = true;
    m_dragStartX = x;
    m_dragStartSample = m_viewStart;
}

void WaveformDisplay::mouseUp(int x, int y, UINT button)
{
    m_isDragging = false;
}

void WaveformDisplay::mouseDrag(int x, int y)
{
    if (!m_isDragging)
        return;
        
    int deltaX = m_dragStartX - x;
    int width = m_bounds.right - m_bounds.left;
    int viewLength = m_viewEnd - m_viewStart;
    int sampleDelta = (deltaX * viewLength) / width;
    
    int newStart = m_dragStartSample + sampleDelta;
    int newEnd = newStart + viewLength;
    
    if (newStart < 0)
    {
        newStart = 0;
        newEnd = viewLength;
    }
    if (newEnd > m_numSamples)
    {
        newEnd = m_numSamples;
        newStart = newEnd - viewLength;
    }
    
    setViewRange(newStart, newEnd);
}

void WaveformDisplay::mouseWheel(int x, int y, int delta)
{
    // Zoom in/out
    float zoomFactor = delta > 0 ? 0.9f : 1.1f;
    int viewLength = m_viewEnd - m_viewStart;
    int newLength = static_cast<int>(viewLength * zoomFactor);
    
    int center = (m_viewStart + m_viewEnd) / 2;
    int newStart = center - newLength / 2;
    int newEnd = center + newLength / 2;
    
    setViewRange(newStart, newEnd);
}

void WaveformDisplay::createWaveformBuffer()
{
    destroyWaveformBuffer();
    
    // Create double buffer for waveform rendering
    // Implementation would go here
}

void WaveformDisplay::destroyWaveformBuffer()
{
    if (m_waveformDC)
    {
        if (m_waveformOldBitmap)
            SelectObject(m_waveformDC, m_waveformOldBitmap);
        if (m_waveformBitmap)
            DeleteObject(m_waveformBitmap);
        DeleteDC(m_waveformDC);
        
        m_waveformDC = nullptr;
        m_waveformBitmap = nullptr;
        m_waveformOldBitmap = nullptr;
    }
}

void WaveformDisplay::renderWaveform()
{
    // Render waveform to buffer
    // Implementation would go here
}

// ComponentManager implementation
ComponentManager::ComponentManager(HWND hwnd)
    : m_hwnd(hwnd)
    , m_rootComponent(nullptr)
    , m_hoveredComponent(nullptr)
    , m_draggedComponent(nullptr)
{
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::setRootComponent(Component* root)
{
    m_rootComponent = root;
    if (root)
    {
        root->setWindowHandle(m_hwnd);
    }
}

void ComponentManager::handleMouseMove(int x, int y)
{
    if (m_draggedComponent)
    {
        m_draggedComponent->mouseDrag(x, y);
        return;
    }
    
    Component* component = findComponentAt(x, y);
    
    if (component != m_hoveredComponent)
    {
        if (m_hoveredComponent)
            m_hoveredComponent->mouseExit();
            
        m_hoveredComponent = component;
        
        if (m_hoveredComponent)
            m_hoveredComponent->mouseEnter();
    }
    
    if (component)
        component->mouseMove(x, y);
}

void ComponentManager::handleMouseDown(int x, int y, UINT button)
{
    Component* component = findComponentAt(x, y);
    if (component)
    {
        component->mouseDown(x, y, button);
        m_draggedComponent = component;
    }
}

void ComponentManager::handleMouseUp(int x, int y, UINT button)
{
    if (m_draggedComponent)
    {
        m_draggedComponent->mouseUp(x, y, button);
        m_draggedComponent = nullptr;
    }
}

void ComponentManager::handleMouseWheel(int x, int y, int delta)
{
    Component* component = findComponentAt(x, y);
    if (component)
    {
        component->mouseWheel(x, y, delta);
    }
}

void ComponentManager::render(HDC hdc)
{
    if (m_rootComponent)
    {
        m_rootComponent->paint(hdc);
        m_rootComponent->paintChildren(hdc);
    }
}

Component* ComponentManager::findComponentAt(int x, int y)
{
    if (m_rootComponent)
        return m_rootComponent->getComponentAt(x, y);
    return nullptr;
}

// DoubleBufferedPainter implementation
DoubleBufferedPainter::DoubleBufferedPainter()
    : m_memDC(nullptr)
    , m_memBitmap(nullptr)
    , m_oldBitmap(nullptr)
    , m_width(0)
    , m_height(0)
{
}

DoubleBufferedPainter::~DoubleBufferedPainter()
{
    cleanup();
}

bool DoubleBufferedPainter::initialize(HWND hwnd, int width, int height)
{
    cleanup();
    
    HDC hdc = GetDC(hwnd);
    m_memDC = CreateCompatibleDC(hdc);
    m_memBitmap = CreateCompatibleBitmap(hdc, width, height);
    m_oldBitmap = (HBITMAP)SelectObject(m_memDC, m_memBitmap);
    ReleaseDC(hwnd, hdc);
    
    m_width = width;
    m_height = height;
    
    return m_memDC != nullptr && m_memBitmap != nullptr;
}

void DoubleBufferedPainter::cleanup()
{
    if (m_memDC)
    {
        if (m_oldBitmap)
            SelectObject(m_memDC, m_oldBitmap);
        if (m_memBitmap)
            DeleteObject(m_memBitmap);
        DeleteDC(m_memDC);
        
        m_memDC = nullptr;
        m_memBitmap = nullptr;
        m_oldBitmap = nullptr;
    }
}

void DoubleBufferedPainter::blitToScreen(HDC screenDC, int width, int height)
{
    if (m_memDC)
    {
        BitBlt(screenDC, 0, 0, width, height, m_memDC, 0, 0, SRCCOPY);
    }
}
