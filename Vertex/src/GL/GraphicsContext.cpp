#include "GraphicsContext.h"

#if defined(VX_RENDER_API_VULKAN)
#else
    #include "OpenGL/OpenGLContext.h"
// ... per rendering API
#endif

namespace Vertex
{
    // clang-format off

#if defined(VX_RENDER_API_VULKAN)

    GraphicsContext* GraphicsContext::Create(GLFWwindow* window_handle)
    {
        return nullptr;
    }

#else

    GraphicsContext* GraphicsContext::Create(GLFWwindow* window_handle)
    {
        return new OpenGLContext(window_handle);
    }

#endif

    // clang-format on
}