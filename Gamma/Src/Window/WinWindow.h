#pragma once

class WinWindow {
    typedef void* HANDLE;
    typedef HANDLE HWND;
    typedef void(*TErrorCallback)(const char* ,const char*);
public:
    void Create();
    void Show();
    bool Update(int& outReturnCode);


    void SetMaxWidth(unsigned width) { m_MaxWidth = width; }
    void SetMaxHeight(unsigned height) { m_MaxHeight = height; }
    void SetErrorCallback(TErrorCallback cb) { m_OnError = cb; }
private:
    const unsigned MAX_WIDTH  = 0xffff;
    const unsigned MAX_HEIGHT = 0xffff;

    HWND m_Hwnd              = 0;
    unsigned m_MaxWidth      = MAX_WIDTH;
    unsigned m_MaxHeight     = MAX_HEIGHT;
    unsigned m_Width         = 1280;
    unsigned m_Height        = 720;
    char* m_Title            = "WinWindow";
    TErrorCallback m_OnError = nullptr;
};