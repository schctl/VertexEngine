#pragma once

#include "Buffer.h"

namespace Vertex
{
    class VertexArray
    {
    public:
        virtual ~VertexArray() { }

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const Ref<VertexBuffer> vbo) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer> ibo)   = 0;

        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static VertexArray* Create();
    };
}