#include "VulkanContext.h"

namespace Vertex
{
    struct UniformBufferObject
    {
        glm::mat4 view_mat;
    };

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

    VulkanContext* VulkanContext::s_Context = nullptr;

    VulkanContext::VulkanContext(GLFWwindow* window) : m_WindowHandle(window)
    {
        VX_CORE_ASSERT(m_WindowHandle, "Window handle is null");

        InitVulkan();

        s_Context = this;
    }

    void VulkanContext::BeginRender()
    {
        vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        VkResult result
            = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX,
                                    m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            VX_CORE_ASSERT(false, "cannot acquire next image");
        }

        // m_PrepareRenderCallback(this);

        if (m_ImagesInFlight[m_CurrentImageIndex] != VK_NULL_HANDLE)
            vkWaitForFences(m_LogicalDevice, 1, &m_ImagesInFlight[m_CurrentImageIndex], VK_TRUE, UINT64_MAX);

        m_ImagesInFlight[m_CurrentImageIndex] = m_InFlightFences[m_CurrentFrame];

        m_CurrentQueueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore          wait_semaphores[]      = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags wait_stages[]          = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        m_CurrentQueueSubmitInfo.waitSemaphoreCount = 1;
        m_CurrentQueueSubmitInfo.pWaitSemaphores    = wait_semaphores;
        m_CurrentQueueSubmitInfo.pWaitDstStageMask  = wait_stages;
        m_CurrentQueueSubmitInfo.commandBufferCount = 1;
        m_CurrentQueueSubmitInfo.pCommandBuffers    = &m_CommandBuffers[m_CurrentImageIndex];

        m_SignalSemaphores[0]                         = { m_RenderFinishedSemaphores[m_CurrentFrame] };
        m_CurrentQueueSubmitInfo.signalSemaphoreCount = 1;
        m_CurrentQueueSubmitInfo.pSignalSemaphores    = m_SignalSemaphores;

        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
        begin_info.pInheritanceInfo = nullptr;                                     // Optional

        if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentImageIndex], &begin_info) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkBeginCommandBuffer failed");

        VkRenderPassBeginInfo render_pass_info {};
        render_pass_info.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass  = m_RenderPass;
        render_pass_info.framebuffer = m_SwapChainFramebuffers[m_CurrentImageIndex];

        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = m_SwapChainExtent;

        VkClearValue clear_color         = { 0.0f, 0.0f, 0.0f, 1.0f };
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues    = &clear_color;

        vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentImageIndex], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentImageIndex];
        m_CurrentDescriptorSet = m_DescriptorSets[m_CurrentImageIndex];
    }

    void VulkanContext::EndRender()
    {
        // m_RenderCallback(this);

        vkCmdEndRenderPass(m_CommandBuffers[m_CurrentImageIndex]);

        if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentImageIndex]) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkEndCommandBuffer failed");

        vkResetFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

        if (vkQueueSubmit(m_GraphicsQueue, 1, &m_CurrentQueueSubmitInfo, m_InFlightFences[m_CurrentFrame])
            != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkQueueSubmit failed");

        VkPresentInfoKHR present_info {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = m_SignalSemaphores;

        VkSwapchainKHR swap_chains[] = { m_SwapChain };
        present_info.swapchainCount  = 1;
        present_info.pSwapchains     = swap_chains;
        present_info.pImageIndices   = &m_CurrentImageIndex;

        present_info.pResults = nullptr; // Optional

        VkResult result = vkQueuePresentKHR(m_PresentQueue, &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            VX_CORE_ASSERT(false, "vkQueuePresentKHR failed");
        }
        vkQueueWaitIdle(m_PresentQueue);

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanContext::InitVulkan()
    {

        CreateInstance();

        if constexpr (EnableValidationLayers)
            InitDebugMessenger();

        CreateSurface();

        PickPhysicalDevice();
        CreateLogicalDevice();

        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateDescriptorSetLayout();
        CreateGraphicsPipelineLayout();

        CreateFramebuffers();

        CreateCommandPool();
        CreateCommandBuffers();

        CreateUniformBuffers();

        CreateSyncObjects();

        CreateDescriptorPool();
        CreateDescriptorSets();

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

        vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);

        vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);

        for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
            vkDestroyImageView(m_LogicalDevice, m_SwapChainImageViews[i], nullptr);

        vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);

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
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipelineLayout();
        CreateFramebuffers();
        CreateCommandBuffers();
        // CreateDescriptorSets();
    }

    // ----------------- Graphics Pipeline ------------------
    // ------------------------------------------------------

    void VulkanContext::CreateRenderPass()
    {
        VkAttachmentDescription color_attachment {};
        color_attachment.format         = m_SwapChainImageFormat;
        color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription sub_pass {};
        sub_pass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sub_pass.colorAttachmentCount = 1;
        sub_pass.pColorAttachments    = &color_attachment_ref;

        VkSubpassDependency dependency {};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_info {};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments    = &color_attachment;
        render_pass_info.subpassCount    = 1;
        render_pass_info.pSubpasses      = &sub_pass;
        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies   = &dependency;

        if (vkCreateRenderPass(m_LogicalDevice, &render_pass_info, nullptr, &m_RenderPass) != VK_SUCCESS)
            throw std::runtime_error("Unable to create render pass");
    }

    void VulkanContext::CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding {};
        ubo_layout_binding.binding         = 0;
        ubo_layout_binding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layout_info {};
        layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings    = &ubo_layout_binding;

        if (vkCreateDescriptorSetLayout(m_LogicalDevice, &layout_info, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor set layout!");

        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts    = &m_DescriptorSetLayout;
    }

    void VulkanContext::CreateGraphicsPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 1;
        pipeline_layout_info.pSetLayouts            = &m_DescriptorSetLayout;
        pipeline_layout_info.pushConstantRangeCount = 0;       // Optional
        pipeline_layout_info.pPushConstantRanges    = nullptr; // Optional

        if (vkCreatePipelineLayout(m_LogicalDevice, &pipeline_layout_info, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            VX_CORE_ASSERT(false, "vkCreatePipelineLayout failed");
        }
    }

    // ------------------------------------------------------
    // ------------------------------------------------------

    void VulkanContext::CreateFramebuffers()
    {
        m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

        for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = { m_SwapChainImageViews[i] };

            VkFramebufferCreateInfo framebuffer_info {};
            framebuffer_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass      = m_RenderPass;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments    = attachments;
            framebuffer_info.width           = m_SwapChainExtent.width;
            framebuffer_info.height          = m_SwapChainExtent.height;
            framebuffer_info.layers          = 1;

            if (vkCreateFramebuffer(m_LogicalDevice, &framebuffer_info, nullptr, &m_SwapChainFramebuffers[i])
                != VK_SUCCESS)
                VX_CORE_ASSERT(false, "vkCreateFramebuffer failed");
        }
    }

    void VulkanContext::CreateCommandPool()
    {
        QueueFamilyIndices queue_family_indices = FindQueueFamilies(m_PhysicalDevice);

        VkCommandPoolCreateInfo pool_info {};
        pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
        pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

        if (vkCreateCommandPool(m_LogicalDevice, &pool_info, nullptr, &m_CommandPool) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkCreateCommandPool failed");
    }

    void VulkanContext::CreateCommandBuffers()
    {
        {
            m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

            VkCommandBufferAllocateInfo alloc_info {};
            alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            alloc_info.commandPool        = m_CommandPool;
            alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_info.commandBufferCount = (uint32_t)m_CommandBuffers.size();

            if (vkAllocateCommandBuffers(m_LogicalDevice, &alloc_info, m_CommandBuffers.data()) != VK_SUCCESS)
                VX_CORE_ASSERT(false, "vkAllocateCommandBuffers failed");
        }
        {
            // allocate the load command buffer
            VkCommandBufferAllocateInfo alloc_info {};
            alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            alloc_info.commandPool        = m_CommandPool;
            alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_info.commandBufferCount = (uint32_t)1;

            if (vkAllocateCommandBuffers(m_LogicalDevice, &alloc_info, &m_LoadCommandBuffer) != VK_SUCCESS)
                VX_CORE_ASSERT(false, "vkAllocateCommandBuffers failed");
        }
    }

    void VulkanContext::CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject); // for now the ubo is just a mat4

        m_UniformBuffers.resize(m_SwapChainImages.size());
        m_UniformBuffersMemory.resize(m_SwapChainImages.size());

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         m_UniformBuffers[i], m_UniformBuffersMemory[i]);
        }
    }

    void VulkanContext::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        m_ImagesInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphore_info {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(m_LogicalDevice, &semaphore_info, nullptr, &m_ImageAvailableSemaphores[i])
                    != VK_SUCCESS
                || vkCreateSemaphore(m_LogicalDevice, &semaphore_info, nullptr, &m_RenderFinishedSemaphores[i])
                    != VK_SUCCESS
                || vkCreateFence(m_LogicalDevice, &fence_info, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                VX_CORE_ASSERT(false, "failed to create synchronization objects for a frame");
            }
        }
    }

    void VulkanContext::CreateDescriptorPool()
    {
        std::vector<VkDescriptorPoolSize> pool_sizes = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                                                         { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets                    = 1000 * pool_sizes.size();
        pool_info.poolSizeCount              = (uint32_t)pool_sizes.size();
        pool_info.pPoolSizes                 = pool_sizes.data();

        if (vkCreateDescriptorPool(m_LogicalDevice, &pool_info, nullptr, &m_DescriptorPool) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkCreateDescriptorPool failed");
    }

    void VulkanContext::CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(m_SwapChainImages.size(), m_DescriptorSetLayout);
        VkDescriptorSetAllocateInfo        allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = m_DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImages.size());
        allocInfo.pSetLayouts        = layouts.data();

        m_DescriptorSets.resize(m_SwapChainImages.size());
        if (vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkAllocateDescriptorSets failed");

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = m_UniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range  = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite {};
            descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet          = m_DescriptorSets[i];
            descriptorWrite.dstBinding      = 0;
            descriptorWrite.dstArrayElement = 0;

            descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo      = &bufferInfo;
            descriptorWrite.pImageInfo       = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(m_LogicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
    }

    // clang-format off
    // ---------------------- Helpers -----------------------

    uint32_t VulkanContext::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &mem_properties);

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

        VX_CORE_ASSERT(false, "failed to find suitable memory type");
    }

    void VulkanContext::CreateBuffer(VkDeviceSize          size,
                                     VkBufferUsageFlags    usage,
                                     VkMemoryPropertyFlags properties,
                                     VkBuffer&             buffer,
                                     VkDeviceMemory&       buffer_memory)
    {
        VkBufferCreateInfo buffer_info {};
        buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size        = size;
        buffer_info.usage       = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_LogicalDevice, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkCreateBuffer failed");

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info {};
        alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize  = mem_requirements.size;
        alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_LogicalDevice, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
            VX_CORE_ASSERT(false, "vkAllocateMemory failed");

        vkBindBufferMemory(m_LogicalDevice, buffer, buffer_memory, 0);
    }

    void VulkanContext::CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
    {
        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(m_LoadCommandBuffer, &begin_info);

        VkBufferCopy copy_region {};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size      = size;
        vkCmdCopyBuffer(m_LoadCommandBuffer, src_buffer, dst_buffer, 1, &copy_region);

        vkEndCommandBuffer(m_LoadCommandBuffer);

        VkSubmitInfo submit_info {};
        submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = &m_LoadCommandBuffer;

        vkQueueSubmit(m_GraphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_GraphicsQueue);
    }

    // ------------------------------------------------------
    // clang-format on

    // -----------------------------------------------------------------

} // namespace Vertex
