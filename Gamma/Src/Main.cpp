

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "Window/WinWindow.h"



int main()
{
    int ret = 0;
    WinWindow window;
    window.Create();
    if (!window.InitializeOpenGL())
        return -1;
    window.Show();
    while (window.Update(ret)) {  }
    return ret;
}
