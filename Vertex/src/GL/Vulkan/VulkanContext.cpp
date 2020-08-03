#include "VulkanContext.h"

namespace Vertex
{
    VulkanContext::VulkanContext(GLFWwindow* window) : m_WindowHandle(window)
    {
        VX_CORE_ASSERT(m_WindowHandle, "Window handle is null");
    }

    void VulkanContext::SwapBuffers() { }
} // namespace Vertex
