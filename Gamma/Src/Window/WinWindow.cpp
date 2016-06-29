#include "WinWindow.h"

//#define UNICODE 1
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

#define DEFAULT_COLOR_BUFFER_SIZE 32
#define DEFAULT_DEPTH_BUFFER_SIZE 24
#define DEFAULT_STENCIL_BUFFER_SIZE 8

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

    const auto lpszClassName = L"WinWindow";

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
#pragma  warning(disable: 4302)
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
#pragma  warning(default: 4302)
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = lpszClassName;


    if (!RegisterClassEx(&wcex))
    {
        m_OnError("Call to RegisterClassEx failed!", "Win32 Guided Tour");
        return;
    }

    auto a = std::wstring(s2ws(m_Title));

    m_Hwnd = CreateWindow(
        lpszClassName,
        s2ws(m_Title).c_str(),
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

    m_Hdc = GetDC((::HWND)m_Hwnd);
    if (!m_Hdc)
    {
        m_OnError("HDC failed.", "Failed to get device context.");
        return;
    }
}

bool WinWindow::InitializeOpenGL() {
    auto hdc = (::HDC)m_Hdc;
    auto hwnd = (::HWND)m_Hwnd;
    auto hrc = (::HGLRC)m_Hrc;
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = DEFAULT_COLOR_BUFFER_SIZE;
    pfd.cDepthBits = DEFAULT_DEPTH_BUFFER_SIZE;
    pfd.cStencilBits = DEFAULT_STENCIL_BUFFER_SIZE;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (pixelFormat == 0)
    {
        DestroyWindow(hwnd);
        m_OnError("OpenGL failed", "Failed to choose a pixel format.");
        return false;
    }

    if (!SetPixelFormat(hdc, pixelFormat, &pfd))
    {
        DestroyWindow(hwnd);
        m_OnError("OpenGL failed", "Failed to set the pixel format.");
        return false;
    }

    ::HGLRC tempContext = wglCreateContext(hdc);
    if (!tempContext)
    {
        DestroyWindow(hwnd);
        m_OnError("OpenGL failed", "Failed to create temporary context for initialization.");
        return false;
    }
    wglMakeCurrent(hdc, tempContext);

    // Initialize GLEW
    if (GLEW_OK != glewInit())
    {
        wglDeleteContext(tempContext);
        DestroyWindow(hwnd);
        m_OnError("OpenGL failed", "Failed to initialize GLEW.");
        return false;
    }

    if (wglChoosePixelFormatARB && wglCreateContextAttribsARB)
    {
        int attribList[] ={
            WGL_SAMPLES_ARB,  0,
            WGL_SAMPLE_BUFFERS_ARB, 0,
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, DEFAULT_COLOR_BUFFER_SIZE,
            WGL_DEPTH_BITS_ARB, DEFAULT_DEPTH_BUFFER_SIZE,
            WGL_STENCIL_BITS_ARB, DEFAULT_STENCIL_BUFFER_SIZE,
            0
        };

        UINT numFormats;
        if (!wglChoosePixelFormatARB(hdc, attribList, NULL, 1, &pixelFormat, &numFormats) || numFormats == 0)
        {
            bool valid = false;
            

            if (!valid)
            {
                wglDeleteContext(tempContext);
                DestroyWindow(hwnd);
                m_OnError("OpenGL failed", "Failed to choose a pixel format.");
                return false;
            }
        }

       

        // Set final pixel format for window
        if (!SetPixelFormat(hdc, pixelFormat, &pfd))
        {
            m_OnError("OpenGL failed", "Failed to set the pixel format");
            return false;
        }

        // Create our new GL context
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            0
        };

        if (!(hrc = wglCreateContextAttribsARB(hdc, 0, attribs)))
        {
            wglDeleteContext(tempContext);
            m_OnError("OpenGL failed", "Failed to create OpenGL context.");
            return false;
        }

        // Delete the old/temporary context and window
        wglDeleteContext(tempContext);

        // Make the new context current
        if (!wglMakeCurrent(hdc, hrc) || !hrc)
        {
            m_OnError("OpenGL failed", "Failed to make the window current.");
            return false;
        }
    }

    if (!GLEW_ARB_framebuffer_object && GLEW_EXT_framebuffer_object)
    {
        glBindFramebuffer = glBindFramebufferEXT;
        glBindRenderbuffer = glBindRenderbufferEXT;
        glBlitFramebuffer = glBlitFramebufferEXT;
        glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
        glDeleteFramebuffers = glDeleteFramebuffersEXT;
        glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
        glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
        glFramebufferTexture1D = glFramebufferTexture1DEXT;
        glFramebufferTexture2D = glFramebufferTexture2DEXT;
        glFramebufferTexture3D = glFramebufferTexture3DEXT;
        glFramebufferTextureLayer = glFramebufferTextureLayerEXT;
        glGenFramebuffers = glGenFramebuffersEXT;
        glGenRenderbuffers = glGenRenderbuffersEXT;
        glGenerateMipmap = glGenerateMipmapEXT;
        glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
        glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
        glIsFramebuffer = glIsFramebufferEXT;
        glIsRenderbuffer = glIsRenderbufferEXT;
        glRenderbufferStorage = glRenderbufferStorageEXT;
        glRenderbufferStorageMultisample = glRenderbufferStorageMultisampleEXT;
    }

    m_Hrc  = (WinWindow::HGLRC)hrc;
    m_Hwnd = (WinWindow::HWND)hwnd;
    m_Hdc  = (WinWindow::HDC)hdc;

    return true;
}

void WinWindow::Show() {
    ShowWindow((::HWND)m_Hwnd, SW_SHOW);
    UpdateWindow((::HWND)m_Hwnd);
    SwapBuffers((::HDC)m_Hdc);
}

bool WinWindow::Update(int& outReturnCode) {
    // Main message loop:
    MSG msg;
    bool closing = !GetMessage(&msg, NULL, 0, 0);
    if(!closing)
    {
        SwapBuffers((::HDC)m_Hdc);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(closing)
        outReturnCode = (int)msg.wParam;
    else
        outReturnCode = 0;

    return !closing;
}
