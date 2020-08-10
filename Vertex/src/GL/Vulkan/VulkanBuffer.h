#pragma once

#include "GL/Buffer.h"

namespace Vertex
{
    // -----------------------------------
    // ---------- Vertex Buffer ----------
    // -----------------------------------

    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(float* vertices, size_t size, const BufferLayout& layout);
        ~VulkanVertexBuffer();

        void Bind() const override;
        void Unbind() const override;

        inline const BufferLayout& GetLayout() const override { return m_Layout; }

    private:
        VkBuffer       m_InternalVkBuffer;
        VkDeviceMemory m_BufferMemory;

        BufferLayout m_Layout;
    };
} // namespace Vertex
