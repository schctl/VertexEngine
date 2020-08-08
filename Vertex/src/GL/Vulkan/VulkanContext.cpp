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

    void VulkanContext::InitVulkan()
    {

        CreateInstance();

        CoreLogger::Debug("Vulkan ready");
    }

    // ------------------------ Implementations ------------------------

    void VulkanContext::CreateInstance()
    {
        // ------------ app info -------------

        VkApplicationInfo app_info {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        // -----------------------------------

        // -----------------------------------
        // ---------- instance info ----------

        VkInstanceCreateInfo instance_info {};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        instance_info.pApplicationInfo = &app_info;

        // --------- extension info ----------

        std::vector<const char*> required_extensions = GetRequiredExtensions();
        instance_info.enabledExtensionCount          = (uint32_t)(required_extensions.size());
        instance_info.ppEnabledExtensionNames        = required_extensions.data();

        // -----------------------------------

        // -------- validation layers --------

        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info {};

        if constexpr (EnableValidationLayers)
        {
            if (!CheckValidationLayerSupport())
            {
                throw std::runtime_error("Validation layers not available.");
            }
            else
            {
                instance_info.enabledLayerCount   = (uint32_t)(ValidationLayers.size());
                instance_info.ppEnabledLayerNames = ValidationLayers.data();

                debug_messenger_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debug_messenger_info.flags           = 0;
                debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debug_messenger_info.pfnUserCallback = VulkanDebugCallback;

                instance_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debug_messenger_info);
            }
        }
        else
        {
            instance_info.enabledLayerCount = 0;
            instance_info.pNext             = nullptr;
        }

        // -----------------------------------

        if (vkCreateInstance(&instance_info, nullptr, &m_Instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create instance");

        // -----------------------------------
        // -----------------------------------
    }

    bool VulkanContext::CheckValidationLayerSupport()
    {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char* required_layer : ValidationLayers)
        {
            bool layer_found = false;

            for (const auto& layer_properties : available_layers)
                if (strcmp(required_layer, layer_properties.layerName) == 0)
                    layer_found = true;

            if (!layer_found)
                return false;
        }

        return true;
    }

    std::vector<const char*> VulkanContext::GetRequiredExtensions()
    {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (EnableValidationLayers)
            extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    // -----------------------------------------------------------------

} // namespace Vertex
