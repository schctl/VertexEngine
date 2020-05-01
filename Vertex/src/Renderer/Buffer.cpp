#include "Buffer.h"

#if defined(VX_RENDER_API_OPENGL)
#include "GL/OpenGL/OpenGLBuffer.h"
#endif

namespace Vertex {

    VertexBuffer* VertexBuffer::Create(float* vertices, size_t size)
    {
#if defined(VX_RENDER_API_OPENGL)
        return new OpenGLVertexBuffer(vertices, size);
#elif defined(VX_RENDER_API_VULKAN)
        return nullptr;//for now
#endif
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, size_t size)
    {
#if defined(VX_RENDER_API_OPENGL)
        return new OpenGLIndexBuffer(indices, size);
#elif defined(VX_RENDER_API_VULKAN)
        return nullptr;//for now
#endif
    }

}