//
// Created by Maks930 on 08/24/2025.
//

#include "Monitor.h"

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#endif //_WIN32

#ifdef __linux__
#include <X11/Xlib.h>
#endif //__linux__


namespace bde::core::monitor {

#ifdef _WIN32
    WinSize getPrimaryMonitorSize() {
        return WinSize{GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    }
#endif //_WIN32

#ifdef __linux__
    WinSize getPrimaryMonitorSize() {
        Display* display = XOpenDisplay(NULL);
        Screen* screen = DefaultScreenOfDisplay(display);

        return WinSize{screen->width,screen->height};
    }
#endif //__linux__
}