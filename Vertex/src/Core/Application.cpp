#include "Application.h"

namespace Vertex {

    Application::Application()
    {
        m_Window.reset(new WindowImpl());
        m_Window->SetEventCallback(VX_BIND_FUNC_1(Application::OnEvent));
    }

    Application::~Application()
    {
    }

    void Application::OnEvent(Event& event)
    {
        Logger::GetCoreLogger()->info(event.GetDetails());

        EventHandler handler(event);
        handler.Dispatch(&Application::OnWindowCloseEvent);
    }

    void Application::Run()
    {
        while (!m_Running)
        {
            m_Window->OnUpdate();
        }
    }

    // Application specific event callbacks
    bool Application::OnWindowCloseEvent(Event& event)
    {
        m_Running = false;
    }

}