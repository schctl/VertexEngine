#pragma once

#include "GL/VertexArray.h"

namespace Vertex
{
    class VulkanVertexArray : public VertexArray
    {
    public:
        VulkanVertexArray();
        ~VulkanVertexArray();

        void Bind() const override;
        void Unbind() const override;

        void AddVertexBuffer(const Ref<VertexBuffer> vbo) override;
        void SetIndexBuffer(const Ref<IndexBuffer> ibo) override;

        inline const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IBO; }

    private:
        std::vector<Ref<VertexBuffer>> m_VBOs;
        Ref<IndexBuffer>               m_IBO;
    };
} // namespace Vertex
