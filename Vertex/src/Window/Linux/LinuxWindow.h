#pragma once

#include "Window/Window.h"

#include "Window/Input.h"

#include "LinuxInput.h"

namespace Vertex
{
    class LinuxWindow : public Window
    {
    public:
        LinuxWindow(const WindowProperties properties = WindowProperties());
        ~LinuxWindow();

        void OnEvent(Event& event) override;

        void OnUpdate(TimeDelta delta_time) override;

        unsigned int GetWidth() const override { return m_Data.width; }
        unsigned int GetHeight() const override { return m_Data.height; }

        inline void SetEventCallback(std::function<void(Event&)> func) override { m_Data.event_callback = func; }

        inline void SetVSync(bool conf) override
        {
#if defined(VX_RENDER_API_VULKAN)

#else
            glfwSwapInterval((conf) ? 1 : 0);
#endif
            m_Data.v_sync = conf;
        }

        inline bool IsVSync() const override { return m_Data.v_sync; }

        inline void* GetNativeWindow() const override { return m_Window; }

        inline bool ShouldClose() const override { return glfwWindowShouldClose(m_Window); }

        inline GraphicsContext& GetGraphicsContext() const override { return *m_Context; }

    private:
        bool OnWindowResizeEvent(WindowResizeEvent& event);

    private:
        GLFWwindow*                      m_Window;
        std::shared_ptr<GraphicsContext> m_Context;
        WindowProperties                 m_Data;

    private:
        void ShutDown();
    };

}