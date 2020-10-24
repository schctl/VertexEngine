#include "VertexArray.h"

#if defined(VX_RENDER_API_VULKAN)
    #include "Vulkan/VulkanVertexArray.h"
#else
    #include "OpenGL/OpenGLVertexArray.h"
// .. per rendering api
#endif

namespace Vertex
{
    // clang-format off

#if defined(VX_RENDER_API_VULKAN)

    VertexArray* VertexArray::Create() { return new VulkanVertexArray(); }

#else

    VertexArray* VertexArray::Create() { return new OpenGLVertexArray(); }

#endif

    // clang format on
}