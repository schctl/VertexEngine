#pragma once

#include "Core/Core.h"

#include "GL/VertexArray.h"

#include "GL/Shader.h"

namespace Vertex
{

    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        ~OpenGLVertexArray();

        void Bind() const override;
        void Unbind() const override;

        void AddVertexBuffer(const Ref<VertexBuffer> vbo) override;
        void SetIndexBuffer(const Ref<IndexBuffer> ibo) override;

        inline const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IBO; }

    private:
        uint32_t m_ID;

        std::vector<Ref<VertexBuffer>> m_VBOs;
        Ref<IndexBuffer>               m_IBO;
    };

}