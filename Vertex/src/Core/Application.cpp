#include "Application.h"

#include "GL/OpenGL/OpenGLShader.h"

namespace Vertex
{

    Application* Application::s_AppInstance = nullptr;

    Application::Application() : m_Running(true)
    {
        VX_CORE_ASSERT((!s_AppInstance), "Application cannot be instantiated twice!");

        m_Window.reset(Window::Create());

        m_Window->SetEventCallback(VX_BIND_FUNC_1(Application::OnEvent));

        s_AppInstance = this;

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        for (Layer* layer : m_LayerStack)
            PopLayer(layer);
    }

    void Application::OnEvent(Event& event)
    {
        EventHandler handler(event);
        handler.Dispatch<EventTypes::WindowClose, WindowCloseEvent>(VX_BIND_FUNC_1(Application::OnWindowCloseEvent));

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
            m_DeltaTime = Time::GetTime() - m_LastFrameTime;
            m_LastFrameTime += m_DeltaTime.GetSeconds();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(m_DeltaTime);

            m_ImGuiLayer->Begin();

            for (Layer* layer : m_LayerStack)
                layer->OnGUIRender();

            m_ImGuiLayer->End();

            m_Window->OnUpdate(m_DeltaTime);
        }
    }

    // Application specific event callbacks
    bool Application::OnWindowCloseEvent(WindowCloseEvent& event)
    {
        m_Running = false;
        return true;
    }

}
