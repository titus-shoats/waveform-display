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

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitializeDoubleBuffering(HWND hwnd);
void CleanupDoubleBuffering();
void PaintDoubleBuffered(HWND hwnd);

// Global variables for double buffering
HDC g_hdcMem = NULL;
HBITMAP g_hbmMem = NULL;
HBITMAP g_hbmOld = NULL;
int g_clientWidth = 0;
int g_clientHeight = 0;

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

void PaintDoubleBuffered(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (g_hdcMem)
    {
        // Clear background
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        FillRect(g_hdcMem, &rcClient, (HBRUSH)(COLOR_WINDOW + 1));

        // Draw to memory DC here
        // This is where all custom drawing will happen
        // For now, just draw a sample text
        SetBkMode(g_hdcMem, TRANSPARENT);
        SetTextColor(g_hdcMem, RGB(0, 0, 0));
        
        const wchar_t* text = L"Just a Sample - Win32 Version";
        RECT textRect = rcClient;
        DrawText(g_hdcMem, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

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
            return 0;
        }

        case WM_SIZE:
        {
            // Reinitialize double buffering when window is resized
            // This ensures the back buffer matches the new window size
            InitializeDoubleBuffering(hwnd);
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

        case WM_DESTROY:
        {
            // Clean up double buffering resources
            CleanupDoubleBuffering();
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
