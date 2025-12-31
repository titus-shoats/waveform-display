/*
  ==============================================================================

    main.cpp
    Win32 API Entry Point for Just a Sample
    
    This file contains the Win32 application entry point and main window procedure.
    Implements double buffering and best practices to prevent flickering.

  ==============================================================================
*/

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string>
#include <memory>

#include "GUIComponents.h"
#include "SamplerProcessor.h"

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitializeDoubleBuffering(HWND hwnd);
void CleanupDoubleBuffering();
void PaintDoubleBuffered(HWND hwnd);
void CreateUI(HWND hwnd);
void CleanupUI();

// Global variables for double buffering
HDC g_hdcMem = NULL;
HBITMAP g_hbmMem = NULL;
HBITMAP g_hbmOld = NULL;
int g_clientWidth = 0;
int g_clientHeight = 0;

// Global UI and audio components
std::unique_ptr<ComponentManager> g_componentManager;
std::unique_ptr<Component> g_rootComponent;
std::unique_ptr<SamplerProcessor> g_audioProcessor;

// Application class name
const wchar_t g_szClassName[] = L"JustASampleWin32Class";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    // Register the Window Class
    // Note: NOT using CS_HREDRAW | CS_VREDRAW to prevent full window redraws on resize
    // This is a best practice for better performance
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;  // No CS_HREDRAW/CS_VREDRAW as per best practices
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the Window
    // Using WS_CLIPCHILDREN to prevent parent from painting over child windows
    // This is essential for proper rendering of child controls
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"Just a Sample - Win32 Audio Sampler",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,  // WS_CLIPCHILDREN is critical
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message Loop
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void InitializeDoubleBuffering(HWND hwnd)
{
    // Clean up any existing resources
    CleanupDoubleBuffering();

    // Get client area dimensions
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    g_clientWidth = rcClient.right - rcClient.left;
    g_clientHeight = rcClient.bottom - rcClient.top;

    // Create memory DC for double buffering
    HDC hdc = GetDC(hwnd);
    g_hdcMem = CreateCompatibleDC(hdc);
    g_hbmMem = CreateCompatibleBitmap(hdc, g_clientWidth, g_clientHeight);
    g_hbmOld = (HBITMAP)SelectObject(g_hdcMem, g_hbmMem);
    ReleaseDC(hwnd, hdc);
}

void CleanupDoubleBuffering()
{
    if (g_hdcMem)
    {
        if (g_hbmOld)
        {
            SelectObject(g_hdcMem, g_hbmOld);
            g_hbmOld = NULL;
        }
        if (g_hbmMem)
        {
            DeleteObject(g_hbmMem);
            g_hbmMem = NULL;
        }
        DeleteDC(g_hdcMem);
        g_hdcMem = NULL;
    }
}

void CreateUI(HWND hwnd)
{
    // Initialize component manager
    g_componentManager = std::make_unique<ComponentManager>(hwnd);
    
    // Create root component
    g_rootComponent = std::make_unique<Component>();
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    g_rootComponent->setBounds(0, 0, rcClient.right, rcClient.bottom);
    g_componentManager->setRootComponent(g_rootComponent.get());
    
    // Create sample UI components (placeholder for full UI)
    Label* titleLabel = new Label(L"Just a Sample - Win32 Audio Sampler");
    titleLabel->setBounds(20, 20, 400, 30);
    titleLabel->setAlignment(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    g_rootComponent->addChildComponent(titleLabel);
    
    Label* infoLabel = new Label(L"Double-buffered Win32 implementation with WASAPI audio");
    infoLabel->setBounds(20, 60, 500, 20);
    infoLabel->setTextColor(RGB(100, 100, 100));
    g_rootComponent->addChildComponent(infoLabel);
    
    // Add load button
    Button* loadButton = new Button(L"Load Sample");
    loadButton->setBounds(20, 100, 120, 30);
    loadButton->setClickCallback([]() {
        MessageBox(NULL, L"File chooser would open here", L"Info", MB_OK);
    });
    g_rootComponent->addChildComponent(loadButton);
    
    // Add play button
    Button* playButton = new Button(L"Play");
    playButton->setBounds(150, 100, 80, 30);
    playButton->setClickCallback([]() {
        if (g_audioProcessor && g_audioProcessor->isAudioInitialized())
        {
            g_audioProcessor->playTestNote();
        }
    });
    g_rootComponent->addChildComponent(playButton);
    
    // Add gain slider
    Label* gainLabel = new Label(L"Gain");
    gainLabel->setBounds(20, 150, 60, 30);
    g_rootComponent->addChildComponent(gainLabel);
    
    Slider* gainSlider = new Slider(Slider::Rotary);
    gainSlider->setBounds(90, 150, 60, 60);
    gainSlider->setRange(0.0f, 1.0f);
    gainSlider->setValue(0.75f);
    gainSlider->setValueChangedCallback([](float value) {
        if (g_audioProcessor)
        {
            g_audioProcessor->setGain(value);
        }
    });
    g_rootComponent->addChildComponent(gainSlider);
    
    // Add waveform display
    WaveformDisplay* waveform = new WaveformDisplay();
    waveform->setBounds(20, 230, rcClient.right - 40, 200);
    g_rootComponent->addChildComponent(waveform);
}

void CleanupUI()
{
    g_componentManager.reset();
    g_rootComponent.reset();
}

void PaintDoubleBuffered(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (g_hdcMem)
    {
        // Clear background with dark gray
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        HBRUSH bgBrush = CreateSolidBrush(RGB(45, 45, 48));
        FillRect(g_hdcMem, &rcClient, bgBrush);
        DeleteObject(bgBrush);

        // Render all components if component manager exists
        if (g_componentManager)
        {
            g_componentManager->render(g_hdcMem);
        }

        // Copy the memory DC to the screen DC (double buffer blit)
        // This eliminates flicker by doing all drawing off-screen first
        BitBlt(hdc, 0, 0, g_clientWidth, g_clientHeight, g_hdcMem, 0, 0, SRCCOPY);
    }

    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            // Initialize double buffering on window creation
            InitializeDoubleBuffering(hwnd);
            
            // Initialize audio processor
            g_audioProcessor = std::make_unique<SamplerProcessor>();
            if (g_audioProcessor->initializeAudio(44100, 512))
            {
                g_audioProcessor->initializeMidi();
                g_audioProcessor->startPlayback();
            }
            
            // Create UI components
            CreateUI(hwnd);
            
            return 0;
        }

        case WM_SIZE:
        {
            // Reinitialize double buffering when window is resized
            // This ensures the back buffer matches the new window size
            InitializeDoubleBuffering(hwnd);
            
            // Update root component size
            if (g_rootComponent)
            {
                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
                g_rootComponent->setBounds(0, 0, rcClient.right, rcClient.bottom);
            }
            
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_PAINT:
        {
            // Use double buffering for all paint operations
            // This prevents flickering during redraws
            PaintDoubleBuffered(hwnd);
            return 0;
        }

        case WM_ERASEBKGND:
        {
            // Don't erase background - we handle it in double buffered paint
            // This prevents flickering
            return 1;
        }
        
        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (g_componentManager)
            {
                g_componentManager->handleMouseDown(x, y, MK_LBUTTON);
            }
            return 0;
        }
        
        case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (g_componentManager)
            {
                g_componentManager->handleMouseUp(x, y, MK_LBUTTON);
            }
            return 0;
        }
        
        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (g_componentManager)
            {
                g_componentManager->handleMouseMove(x, y);
            }
            return 0;
        }
        
        case WM_MOUSEWHEEL:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (g_componentManager)
            {
                g_componentManager->handleMouseWheel(x, y, delta);
            }
            return 0;
        }

        case WM_DESTROY:
        {
            // Clean up audio processor
            if (g_audioProcessor)
            {
                g_audioProcessor->stopPlayback();
                g_audioProcessor->releaseAudio();
                g_audioProcessor->releaseMidi();
                g_audioProcessor.reset();
            }
            
            // Clean up UI
            CleanupUI();
            
            // Clean up double buffering resources
            CleanupDoubleBuffering();
            
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
