#include "Application.h"

#include "GL/OpenGL/OpenGLShader.h"

namespace Vertex
{
    Application* Application::s_AppInstance = nullptr;

    Application::Application(const char* name) : m_Running(true)
    {
        VX_CORE_ASSERT((!s_AppInstance), "Application cannot be instantiated twice!");

        m_Window.reset(Window::Create({ name }));

        m_Window->SetEventCallback(VX_BIND_FUNC_1(Application::OnEvent));

        s_AppInstance = this;

        m_GUILayer = new GUILayer();
        PushOverlay(m_GUILayer);
    }

    Application::~Application() { delete m_GUILayer; }

    void Application::OnEvent(Event& event)
    {
        EventHandler handler(event);

        handler.Dispatch<WindowCloseEvent>(VX_BIND_FUNC_1(Application::OnWindowCloseEvent));

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
            m_LastFrameTime += m_DeltaTime.TotalSeconds();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(m_DeltaTime);

            m_GUILayer->Begin();

            for (Layer* layer : m_LayerStack)
                layer->OnGUIRender(m_DeltaTime);

            m_GUILayer->End();

            m_Window->OnUpdate(m_DeltaTime);
        }

        for (Layer* layer : m_LayerStack)
            PopLayer(layer);
    }

    // Application specific event callbacks
    bool Application::OnWindowCloseEvent(WindowCloseEvent& event)
    {
        m_Running = false;
        return true;
    }
}
