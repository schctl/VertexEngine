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

    // ----------------------------------
    // ---------- Index Buffer ----------
    // ----------------------------------

    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        VulkanIndexBuffer(uint32_t* indices, size_t size);
        ~VulkanIndexBuffer();

        void Bind() const override;
        void Unbind() const override;

        uint32_t GetCount() const override { return m_Count; }

    private:
        VkBuffer       m_InternalVkBuffer;
        VkDeviceMemory m_BufferMemory;
        uint32_t       m_Count;
    };
} // namespace Vertex
