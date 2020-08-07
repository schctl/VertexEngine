#include "GraphicsAPI.h"

#if defined(VX_RENDER_API_VULKAN)
#else
    #include "OpenGL/OpenGLGraphicsAPI.h"
// ... per rendering API
#endif

namespace Vertex
{
    // clang-format off

#if defined(VX_RENDER_API_VULKAN)

    GraphicsAPI* GraphicsAPI::Create()
    {
        return nullptr;
    }

#else

    GraphicsAPI* GraphicsAPI::Create()
    {
        return new OpenGLGraphicsAPI();
    }

#endif

    // clang-format on
}