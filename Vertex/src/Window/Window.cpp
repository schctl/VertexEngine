#include "Window.h"

#if defined(_WIN32)
    #include "Windows/WindowsWindow.h"
#elif defined(__linux__)
    #include "Linux/LinuxWindow.h"
#endif

namespace Vertex
{
    Window* Window::Create(const WindowProperties properties)
    {
#if defined(_WIN32)
        return new WindowsWindow(properties);
#elif defined(__linux__)
        return new LinuxWindow(properties);
#endif
    }
}