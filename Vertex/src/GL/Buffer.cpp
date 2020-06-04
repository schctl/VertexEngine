#include "Buffer.h"

#include "OpenGL/OpenGLBuffer.h"
// ... per rendering API

namespace Vertex
{
    VertexBuffer* VertexBuffer::Create(float* vertices, size_t size, const BufferLayout& layout)
    {
        return new OpenGLVertexBuffer(vertices, size, layout);
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, size_t size) { return new OpenGLIndexBuffer(indices, size); }
}