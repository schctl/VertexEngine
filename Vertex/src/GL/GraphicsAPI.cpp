#include "GraphicsAPI.h"

#if defined(VX_RENDER_API_VULKAN)
    #include "Vulkan/VulkanGraphicsAPI.h"
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
        return new VulkanGraphicsAPI();
    }

#else

    GraphicsAPI* GraphicsAPI::Create()
    {
        return new OpenGLGraphicsAPI();
    }

#endif

    // clang-format on
}