#include "VulkanContext.h"

namespace Vertex
{
    // -----------------------------------------------------------------

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

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info)
    {
        create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = VulkanDebugCallback;
    }

    static VkResult CreateDebugUtilsMessengerEXT
    (
        VkInstance                                instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks*              pAllocator,
        VkDebugUtilsMessengerEXT*                 pDebugMessenger
    )
    {
        auto func
            = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    static void DestroyDebugUtilsMessengerEXT
    (
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    )
    {
        auto func
            = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

        if (func != nullptr)
            func(instance, debugMessenger, pAllocator);
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

    // -----------------------------------------------------------------

    VulkanContext::VulkanContext(GLFWwindow* window) : m_WindowHandle(window)
    {
        VX_CORE_ASSERT(m_WindowHandle, "Window handle is null");

        InitVulkan();
    }

    void VulkanContext::SwapBuffers() { }

    void VulkanContext::InitVulkan()
    {

        CreateInstance();

        if constexpr (EnableValidationLayers)
            InitDebugMessenger();

        CreateSurface();

        PickPhysicalDevice();

        CreateLogicalDevice();

        // ------------- swapchain -------------

        CreateSwapChain();

        CreateImageViews();
        //
        // CreateRenderPass();
        //
        // CreateDescriptorSetLayout();
        //
        // CreateGraphicsPipeline();
        //
        // CreateFramebuffers();
        //
        // CreateCommandPool();
        //
        // CreateVertexBuffer();
        //
        // CreateIndexBuffer();
        //
        // CreateUniformBuffers();
        //
        // CreateDescriptorPool();
        //
        // CreateDescriptorSets();
        //
        // CreateCommandBuffers();
        //
        // CreateSyncObjects();

        // -------------------------------------

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

                PopulateDebugMessengerCreateInfo(debug_messenger_info);

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

    void VulkanContext::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queue_info_list;
        std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

        float queue_priority = 1.0f;

        for (const auto& unique_queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_info {};
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

            queue_info.queueFamilyIndex = unique_queue_family;
            queue_info.queueCount       = 1;

            queue_info.pQueuePriorities = &queue_priority;

            queue_info_list.emplace_back(queue_info);
        }

        VkPhysicalDeviceFeatures physical_device_features {};

        VkDeviceCreateInfo device_info {};
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        device_info.queueCreateInfoCount = (uint32_t)(queue_info_list.size());
        device_info.pQueueCreateInfos    = queue_info_list.data();

        device_info.pEnabledFeatures = &physical_device_features;

        device_info.enabledExtensionCount   = (uint32_t)(VulkanDeviceExtensions.size());
        device_info.ppEnabledExtensionNames = VulkanDeviceExtensions.data();

        // ----- Backwards compatibility -----

        if constexpr (EnableValidationLayers)
        {
            device_info.enabledLayerCount = (uint32_t)(
                ValidationLayers
                    .size()); // Previous implementations of Vulkan made a distinction between instance and device
                              // specific validation layers, but this is no longer the case. That means that the
                              // enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by
                              // up-to-date implementations. However, it is still a good idea to set them anyway to be
                              // compatible with older implementations:
            device_info.ppEnabledLayerNames = ValidationLayers.data();
        }
        else
        {
            device_info.enabledLayerCount = 0;
        }

        // -----------------------------------

        if (vkCreateDevice(m_PhysicalDevice, &device_info, nullptr, &m_LogicalDevice) != VK_SUCCESS)
            throw std::runtime_error("Failed to create logical device");

        vkGetDeviceQueue(m_LogicalDevice, indices.graphics_family.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, indices.present_family.value(), 0, &m_PresentQueue);
    }

    void VulkanContext::CreateSurface()
    {
        if (glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface) != VK_SUCCESS)
            throw std::runtime_error("Unable to create window surface");
    }

    void VulkanContext::InitDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT create_info;

        PopulateDebugMessengerCreateInfo(create_info);

        if (CreateDebugUtilsMessengerEXT(m_Instance, &create_info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
            throw std::runtime_error("failed to set up debug messenger!");
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

    // ------------------------------------------------------
    // ------------------- Physical Device ------------------

    /**
     * @todo Rate physical device "scores" and choose best option.
     */
    void VulkanContext::PickPhysicalDevice()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_Instance, &device_count, nullptr);

        if (device_count == 0)
            throw std::runtime_error("Unable to find any physical devices wih vulkan support");

        std::vector<VkPhysicalDevice> physical_devices(device_count);
        vkEnumeratePhysicalDevices(m_Instance, &device_count, physical_devices.data());

        for (const auto& device : physical_devices)
            if (IsDeviceSuitable(device))
            {
                m_PhysicalDevice = device;
                break;
            }

        if (m_PhysicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("Unable to find any suitable physical devices");
    }

    bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensions_supported = CheckDeviceExtensionSupport(device);

        bool swapchain_adequate = false;

        if (extensions_supported)
        {
            SwapChainSupportDetails details = QuerySwapChainSupportDetails(device);
            swapchain_adequate              = !details.formats.empty() && !details.present_modes.empty();
        }

        return indices.IsComplete() && extensions_supported && swapchain_adequate;
    }

    VulkanContext::QueueFamilyIndices VulkanContext::FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        uint32_t i = 0;

        for (const auto& queue_family : queue_families)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphics_family = i;

            VkBool32 present_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &present_support);

            if (present_support)
                indices.present_family = i;

            if (indices.IsComplete())
                break;

            i++;
        }

        return indices;
    }

    bool VulkanContext::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> required_extensions(VulkanDeviceExtensions.begin(), VulkanDeviceExtensions.end());

        for (const auto& extension : available_extensions)
            required_extensions.erase(extension.extensionName);

        return required_extensions.empty();
    }

    // ------------------------------------------------------
    // --------------------- Swapchain ----------------------

    VulkanContext::SwapChainSupportDetails VulkanContext::QuerySwapChainSupportDetails(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &format_count, nullptr);

        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &format_count, details.formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &present_mode_count, nullptr);

        if (present_mode_count != 0)
        {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &present_mode_count,
                                                      details.present_modes.data());
        }

        return details;
    }

    void VulkanContext::CreateSwapChain()
    {
        SwapChainSupportDetails details = QuerySwapChainSupportDetails(m_PhysicalDevice);

        VkSurfaceFormatKHR surface_format = ChooseSwapChainFormat(details.formats);
        VkPresentModeKHR   present_mode   = ChooseSwapPresentMode(details.present_modes);
        VkExtent2D         swap_extent    = ChooseSwapExtent(details.capabilities);

        uint32_t image_count = details.capabilities.minImageCount + 1;

        if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount)
            image_count = details.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR swap_chain_info {};
        swap_chain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

        swap_chain_info.surface          = m_Surface;
        swap_chain_info.minImageCount    = image_count;
        swap_chain_info.imageFormat      = surface_format.format;
        swap_chain_info.imageColorSpace  = surface_format.colorSpace;
        swap_chain_info.imageExtent      = swap_extent;
        swap_chain_info.imageArrayLayers = 1;
        swap_chain_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        if (indices.graphics_family != indices.present_family)
        {
            uint32_t queue_families[] = { indices.graphics_family.value(), indices.present_family.value() };

            swap_chain_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            swap_chain_info.queueFamilyIndexCount = 2;
            swap_chain_info.pQueueFamilyIndices   = queue_families;
        }
        else
        {
            swap_chain_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            swap_chain_info.queueFamilyIndexCount = 0;
            swap_chain_info.pQueueFamilyIndices   = nullptr;
        }

        swap_chain_info.preTransform   = details.capabilities.currentTransform;
        swap_chain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_chain_info.presentMode    = present_mode;
        swap_chain_info.clipped        = VK_TRUE;
        swap_chain_info.oldSwapchain   = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_LogicalDevice, &swap_chain_info, nullptr, &m_SwapChain) != VK_SUCCESS)
            throw std::runtime_error("Unable to create swapchain");

        vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &image_count, nullptr);
        m_SwapChainImages.resize(image_count);
        vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &image_count, m_SwapChainImages.data());

        m_SwapChainImageFormat = surface_format.format;
        m_SwapChainExtent      = swap_extent;
    }

    VkSurfaceFormatKHR VulkanContext::ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        for (const auto& format : available_formats)
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
                return format;

        return available_formats[0]; // eh
    }

    VkPresentModeKHR VulkanContext::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
    {
        for (const auto& present_mode : available_present_modes)
            if (present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
                return present_mode;

        return VK_PRESENT_MODE_FIFO_KHR; // guaranteed
    }

    VkExtent2D VulkanContext::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
            return capabilities.currentExtent;

        int _ac_width, _ac_height;
        glfwGetWindowSize(m_WindowHandle, &_ac_width, &_ac_height);

        VkExtent2D actual_extent = { (uint32_t)_ac_width, (uint32_t)_ac_height };

        actual_extent.width  = std::max(capabilities.minImageExtent.width,
                                       std::min(capabilities.maxImageExtent.width, actual_extent.width));
        actual_extent.height = std::max(capabilities.minImageExtent.height,
                                        std::min(capabilities.maxImageExtent.height, actual_extent.height));

        return actual_extent;
    }

    void VulkanContext::CreateImageViews()
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo image_view_info {};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

            image_view_info.image    = m_SwapChainImages[i];
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_info.format   = m_SwapChainImageFormat;

            image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_info.subresourceRange.baseMipLevel   = 0;
            image_view_info.subresourceRange.levelCount     = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_LogicalDevice, &image_view_info, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create image views");
        }
    }

    void VulkanContext::CleanupSwapchain()
    {
        vkDeviceWaitIdle(m_LogicalDevice);

        // for (size_t i = 0; i < m_SwapChainFramebuffers.size(); i++)
        //     vkDestroyFramebuffer(m_LogicalDevice, m_SwapChainFramebuffers[i], nullptr);
        //
        // vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, (uint32_t)(m_CommandBuffers.size()),
        //                      m_CommandBuffers.data());
        //
        // vkDestroyPipeline(m_LogicalDevice, m_GraphicsPipeline, nullptr);
        //
        // vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
        //
        // vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);
        //
        for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
            vkDestroyImageView(m_LogicalDevice, m_SwapChainImageViews[i], nullptr);
        //
        // vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
        //
        // for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        // {
        //     vkDestroyBuffer(m_LogicalDevice, m_UniformBuffers[i], nullptr);
        //     vkFreeMemory(m_LogicalDevice, m_UniformBuffersMemory[i], nullptr);
        // }
        //
        // vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);
    }

    void VulkanContext::RecreateSwapchain()
    {
        CleanupSwapchain();

        CreateSwapChain();
        // CreateImageViews();
        // CreateRenderPass();
        // CreateGraphicsPipeline();
        // CreateFramebuffers();
        // CreateUniformBuffers();
        // CreateDescriptorSets();
        // CreateCommandBuffers();
    }

    // ------------------------------------------------------
    // ------------------------------------------------------

    // -----------------------------------------------------------------

} // namespace Vertex
