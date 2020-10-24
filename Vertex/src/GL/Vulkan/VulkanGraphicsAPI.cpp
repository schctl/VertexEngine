#include "VulkanGraphicsAPI.h"

namespace Vertex
{
    void VulkanGraphicsAPI::DrawIndexed(Ref<VertexArray> vertex_array)
    {
        VulkanContext::Get()->BeginRender();
        vkCmdDrawIndexed(VulkanContext::Get()->GetCurrentCommandBuffer(),
                         static_cast<uint32_t>(vertex_array->GetIndexBuffer()->GetCount()), 1, 0, 0, 0);
        VulkanContext::Get()->EndRender();
    }
}