#include "Buffer.h"

namespace Vertex {

    VertexBuffer* VertexBuffer::Create(float* vertices, size_t size)
    {
#if defined(VX_RENDER_API_OPENGL)
        return new OpenGLVertexBuffer(vertices, size);
#else
        return nullptr;
#endif
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, size_t size)
    {
#if defined(VX_RENDER_API_OPENGL)
        return new OpenGLIndexBuffer(indices, size);
#else
        return nullptr;
#endif
    }

}