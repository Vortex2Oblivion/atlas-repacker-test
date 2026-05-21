#include "Windows.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <dwmapi.h>

#endif

void setDarkMode()
{
    #ifdef _WIN32
    int darkMode = 1;

    HWND window = GetActiveWindow();
    if (window == NULL)
        return;

    if (S_OK != DwmSetWindowAttribute(window, 19, &darkMode, sizeof(darkMode)))
    {
        DwmSetWindowAttribute(window, 20, &darkMode, sizeof(darkMode));
    }

    UpdateWindow(window);
    #endif
}