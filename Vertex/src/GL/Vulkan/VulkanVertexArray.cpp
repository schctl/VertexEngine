#include "VulkanVertexArray.h"

namespace Vertex
{
    VulkanVertexArray::VulkanVertexArray()  = default;
    VulkanVertexArray::~VulkanVertexArray() = default;

    void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer> vbo) { m_VBOs.emplace_back(vbo); }
    void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer> ibo) { m_IBO = ibo; }

    void VulkanVertexArray::Bind() const
    {
        for (const auto& vbo : m_VBOs)
            vbo->Bind();

        m_IBO->Bind();
    }

    void VulkanVertexArray::Unbind() const { }
} // namespace Vertex