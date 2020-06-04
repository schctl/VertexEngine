#include "Input.h"

#include "Window/Linux/LinuxInput.h"
#include "Window/Windows/WindowsInput.h"

namespace Vertex
{
#if defined(_WIN32)
    Input* Input::s_Instance = new WindowsInput();
#elif defined(__linux__)
    Input* Input::s_Instance = new LinuxInput();
#else
    #error Unsupported platform
#endif
}