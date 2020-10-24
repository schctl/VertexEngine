#include "Application.h"

#include "GL/OpenGL/OpenGLShader.h"

namespace Vertex
{
    Application* Application::s_AppInstance = nullptr;

    Application::Application(const char* name) : m_Running(true), m_Minimized(false)
    {
        VX_CORE_ASSERT((!s_AppInstance), "Application cannot be instantiated twice!");

        m_Window.reset(Window::Create({ name }));

        m_Window->SetEventCallback(VX_BIND_FUNC_1(Application::OnEvent));

        s_AppInstance = this;

#if defined(VX_RENDER_API_VULKAN)
#else
        m_GUILayer = new GUILayer();
        PushOverlay(m_GUILayer);
#endif

        Renderer::Init();
    }

    Application::~Application() { delete m_GUILayer; }

    void Application::OnEvent(Event& event)
    {
        EventHandler handler(event);

        handler.Dispatch<WindowCloseEvent>(VX_BIND_FUNC_1(Application::OnWindowClose));
        handler.Dispatch<WindowResizeEvent>(VX_BIND_FUNC_1(Application::OnWindowResize));

        if (event.GetEventCategory() == EventCategories::WindowEvent)
            m_Window->OnEvent(event);

        for (std::vector<Layer*>::iterator it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(event);

            if (event.IsHandled())
                break;
        }
    }

    void Application::Run()
    {
        while (m_Running)
        {
#if defined(VX_RENDER_API_VULKAN)

            m_DeltaTime = Time::GetTime() - m_LastFrameTime;
            m_LastFrameTime += m_DeltaTime.TotalSeconds();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(m_DeltaTime);

            for (Layer* layer : m_LayerStack)
                layer->OnGUIUpdate(m_DeltaTime);

            m_Window->OnUpdate(m_DeltaTime);

#else
            m_DeltaTime = Time::GetTime() - m_LastFrameTime;
            m_LastFrameTime += m_DeltaTime.TotalSeconds();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(m_DeltaTime);

            m_GUILayer->Begin();

            for (Layer* layer : m_LayerStack)
                layer->OnGUIUpdate(m_DeltaTime);

            m_GUILayer->End();

            m_Window->OnUpdate(m_DeltaTime);

#endif
        }

        for (Layer* layer : m_LayerStack)
            PopLayer(layer);
    }

    // Application specific event callbacks
    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
        if (event.GetWidth() == 0 || event.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;

        // Renderer::NotifyResize(event.GetWidth(), event.GetHeight());

        return false;
    }
}
