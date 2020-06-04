#include "VertexArray.h"

#include "OpenGL/OpenGLVertexArray.h"
// ... per rendering api

namespace Vertex
{
    VertexArray* VertexArray::Create()
    {
#if defined(VX_RENDER_API_OPENGL)
        return new OpenGLVertexArray();
#elif defined(VX_RENDER_API_VULKAN)
        return new VulkanVertexArray();
#else
        return nullptr;
#endif
    }
}