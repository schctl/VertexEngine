#include "VertexArray.h"

#if defined(VX_RENDER_API_VULKAN)
#else
    #include "OpenGL/OpenGLVertexArray.h"
// .. per rendering api
#endif

namespace Vertex
{
    // clang-format off

#if defined(VX_RENDER_API_VULKAN)

    VertexArray* VertexArray::Create() { return nullptr; }

#else

    VertexArray* VertexArray::Create() { return new OpenGLVertexArray(); }

#endif

    // clang format on
}