#include "VulkanBuffer.h"

#include "VulkanContext.h"

namespace Vertex
{
    // -----------------------------------
    // ---------- Vertex Buffer ----------
    // -----------------------------------

    VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, size_t size, const BufferLayout& layout) : m_Layout(layout)
    {
        VulkanContext* context = VulkanContext::Get();

        VkDeviceSize buffer_size = size;

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        // clang-format off

        context->CreateBuffer(buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(context->GetLogicalDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, vertices, size);
        vkUnmapMemory(context->GetLogicalDevice(), staging_buffer_memory);

        context->CreateBuffer(buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     m_InternalVkBuffer, m_BufferMemory);

        // clang-format on

        context->CopyBuffer(staging_buffer, m_InternalVkBuffer, buffer_size);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        VkDevice device = VulkanContext::Get()->GetLogicalDevice();
        vkDestroyBuffer(device, m_InternalVkBuffer, nullptr);
        vkFreeMemory(device, m_BufferMemory, nullptr);
    }

    void VulkanVertexBuffer::Bind() const
    {
        VkBuffer     vertex_buffers[] = { m_InternalVkBuffer };
        VkDeviceSize offsets[]        = { 0 };
        vkCmdBindVertexBuffers(VulkanContext::Get()->GetCurrentCommandBuffer(), 0, 1, vertex_buffers, offsets);
    }

    void VulkanVertexBuffer::Unbind() const { }

} // namespace Vertex