/**
 * @file main.cpp
 * @brief Win32 Entry Point for Just a Sample VST3 Plugin
 * 
 * This file contains the main entry point for the standalone Win32 version
 * of the plugin. For VST3 hosts, entry points are in VST3Plugin.cpp
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string>

// Link against common controls library
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
    name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
    processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Window class name
const wchar_t g_szClassName[] = L"JustASampleWindow";
const wchar_t g_szWindowTitle[] = L"Just a Sample - Win32 Version";

// Window dimensions (matching original plugin approximately)
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief Main entry point for Win32 application
 */
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Register window class
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = g_szClassName;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Calculate window size including borders
    RECT windowRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    // Create main window
    HWND hwnd = CreateWindowExW(
        0,
        g_szClassName,
        g_szWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/**
 * @brief Window procedure for handling messages
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Create a simple label to show we're running
        CreateWindowW(
            L"STATIC",
            L"Just a Sample - Win32 Version\n\nStage 1: Basic Window (COMPLETE)",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 50,
            400, 100,
            hwnd, NULL,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Fill background
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH brush = CreateSolidBrush(RGB(240, 240, 245));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);

        // Draw a simple border
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(100, 100, 120));
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        Rectangle(hdc, 10, 10, rect.right - 10, rect.bottom - 10);
        
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(pen);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
    {
        // Handle window resize
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_CLOSE:
    {
        if (MessageBoxW(hwnd, L"Are you sure you want to close?", L"Confirm",
            MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            DestroyWindow(hwnd);
        }
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
