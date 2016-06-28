#include "WinWindow.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <string>

// http://stackoverflow.com/questions/10737644/convert-const-char-to-wstring
std::wstring s2ws(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}


void GenericErrorHandler(const char* err, const char* description) {
    MessageBox(NULL, s2ws(err).c_str(), s2ws(description).c_str(), NULL);
}

LRESULT CALLBACK wWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, World!");

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // Here your application is laid out.
        // For this introduction, we just print out "Hello, World!"
        // in the top left corner.
        TextOut(hdc,
            5, 5,
            greeting, _tcslen(greeting));
        // End application-specific layout section.

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

void WinWindow::Create() {
    if(!m_OnError)
        m_OnError = GenericErrorHandler;

    HINSTANCE hInstance = ::GetModuleHandle(NULL);

    WNDCLASSEX wcex;

    auto lpszClassName = s2ws("AppWindow").c_str();

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"test";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        m_OnError("Call to RegisterClassEx failed!", "Win32 Guided Tour");
        return;
    }


    m_Hwnd = CreateWindow(
        L"test",
        L"test",//s2ws(m_Title).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        m_Width, m_Height,
        NULL,
        NULL,
        hInstance,
        NULL
        );
    

    if (!m_Hwnd)
    {
        m_OnError("Call to CreateWindow failed!", "Win32 Guided Tour");
        return;
    }
}

void WinWindow::Show() {
    ShowWindow((::HWND)m_Hwnd, SW_SHOW);
    UpdateWindow((::HWND)m_Hwnd);
}

bool WinWindow::Update(int& outReturnCode) {
    // Main message loop:
    MSG msg;
    bool closing = !GetMessage(&msg, NULL, 0, 0);
    if(!closing)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(closing)
        outReturnCode = (int)msg.wParam;
    else
        outReturnCode = 0;

    return !closing;
}
