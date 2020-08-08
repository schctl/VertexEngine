#pragma once

#include "Core/Core.h"

#include "GL/GraphicsContext.h"

#include "VulkanGraphicsAPI.h"

namespace Vertex
{
    class VulkanContext : public GraphicsContext
    {
        friend class VulkanGraphicsAPI;

    public:
        VulkanContext(GLFWwindow* window);

        void SwapBuffers() override;

        void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) const override {};

    private:
        GLFWwindow* m_WindowHandle;

    private:
        void InitVulkan();

    private:
        void CreateInstance();

        bool CheckValidationLayerSupport();

        void CreateSurface();

        void InitDebugMessenger();

        std::vector<const char*> GetRequiredExtensions();

    private:
        VkInstance m_Instance;

        VkSurfaceKHR m_Surface;

        VkDebugUtilsMessengerEXT m_DebugMessenger;

        bool m_NeedSwapChainRecreate = false;
    };
} // namespace Vertex