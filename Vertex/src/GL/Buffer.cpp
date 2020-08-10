#include "Buffer.h"

#if defined(VX_RENDER_API_VULKAN)
    #include "Vulkan/VulkanBuffer.h"
#else
    #include "OpenGL/OpenGLBuffer.h"
// ... per rendering API
#endif

namespace Vertex
{
    // clang-format off

#if defined(VX_RENDER_API_VULKAN)

    VertexBuffer* VertexBuffer::Create(float* vertices, size_t size, const BufferLayout& layout)
    {
        return new VulkanVertexBuffer(vertices, size, layout);
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, size_t size)
    {
        return nullptr;
    }

#else

    VertexBuffer* VertexBuffer::Create(float* vertices, size_t size, const BufferLayout& layout)
    {
        return new OpenGLVertexBuffer(vertices, size, layout);
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, size_t size)
    {
        return new OpenGLIndexBuffer(indices, size);
    }

#endif

    // clang-format on
}