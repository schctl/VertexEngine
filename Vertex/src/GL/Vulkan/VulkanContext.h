#pragma once

#include "Core/Core.h"

#include "GL/GraphicsContext.h"

#include "VulkanGraphicsAPI.h"

namespace Vertex
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(GLFWwindow* window);

        void SwapBuffers() override;

        void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) const override {};

    public:
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphics_family;
            std::optional<uint32_t> present_family;

            bool IsComplete() { return graphics_family.has_value() && present_family.has_value(); }
        };

        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   present_modes;
        };

    private:
        GLFWwindow* m_WindowHandle;

    private:
        void InitVulkan();

    private:
        void CreateInstance();

        bool CheckValidationLayerSupport();

        void CreateSurface();

        void CreateLogicalDevice();

        void InitDebugMessenger();

        std::vector<const char*> GetRequiredExtensions();

        // ------------------------------------------------------
        // ------------------- Physical Device ------------------

        void PickPhysicalDevice();

        bool IsDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice device);

        // ------------------------------------------------------
        // --------------------- Swapchain ----------------------

        VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);

        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void CreateSwapChain();

        void CreateImageViews();

        void CleanupSwapchain();

        void RecreateSwapchain();

        // ------------------------------------------------------
        // ------------------------------------------------------

    private:
        VkInstance m_Instance;

        VkPhysicalDevice m_PhysicalDevice;
        VkDevice         m_LogicalDevice;

        VkSurfaceKHR m_Surface;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        // --------------------- Swapchain ----------------------

        VkSwapchainKHR m_SwapChain;

        std::vector<VkImage> m_SwapChainImages;

        VkFormat   m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;

        std::vector<VkImageView> m_SwapChainImageViews;

        // ------------------------------------------------------

        VkDebugUtilsMessengerEXT m_DebugMessenger;

        bool m_NeedSwapChainRecreate = false;
    };
} // namespace Vertex