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
    }

    void Application::Run()
    {
        while (!m_Window->ShouldClose())
        {
            m_Window->OnUpdate();
        }
    }

}