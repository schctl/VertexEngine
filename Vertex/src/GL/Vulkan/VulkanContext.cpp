#include "VulkanContext.h"

namespace Vertex
{
    // clang-format off

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
        VkDebugUtilsMessageTypeFlagsEXT             message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
        void*                                       p_user_data
    )
    {
        CoreLogger::Debug("Validation Layer: {}", p_callback_data->pMessage);
        return VK_FALSE;
    }

    // clang-format on

#ifdef VX_CONFIGURATION_DEBUG
    constexpr bool EnableValidationLayers = true;
#else
    constexpr bool EnableValidationLayers = false;
#endif

    constexpr int MAX_FRAMES_IN_FLIGHT = 4;

    static const std::vector<const char*> VulkanDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    VulkanContext::VulkanContext(GLFWwindow* window) : m_WindowHandle(window)
    {
        VX_CORE_ASSERT(m_WindowHandle, "Window handle is null");

        InitVulkan();
    }

    void VulkanContext::SwapBuffers() { }

    // ------------------------ Implementations ------------------------

    void VulkanContext::InitVulkan() { CoreLogger::Debug("Vulkan ready"); }

    // -----------------------------------------------------------------

} // namespace Vertex
