#pragma once

#include "Core.h"

class GUILayer; // <- forward declare it to be accessible from application

#include "Event/Event.h"
#include "Event/WindowEvent.h"

#include "Layer/LayerStack.h"

#include "GUI/GUILayer.h"

#include "Time/Time.h"

#include "Window/Window.h"

#include "GL/Buffer.h"
#include "GL/Shader.h"
#include "GL/VertexArray.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"

namespace Vertex
{
    class VX_PUBLIC_API Application
    {
    public:
        Application(const char* name = "VertexEngine");
        ~Application();

        inline Window& GetWindow() { return *m_Window; }

        static inline Application& Get() { return *s_AppInstance; }

        inline void PushLayer(Layer* layer)
        {
            m_LayerStack.PushLayer(layer);
            layer->OnAttach();
        }

        inline void PopLayer(Layer* layer)
        {
            m_LayerStack.PopLayer(layer);
            layer->OnDetach();
        }

        inline void PushOverlay(Layer* overlay)
        {
            m_LayerStack.PushOverlay(overlay);
            overlay->OnAttach();
        }

        inline void PopOverlay(Layer* overlay)
        {
            m_LayerStack.PopOverlay(overlay);
            overlay->OnDetach();
        }

        void OnEvent(Event& event);

        void Run();

    private:
        std::shared_ptr<Window> m_Window;

        LayerStack m_LayerStack;
        GUILayer*  m_GUILayer;

        TimeDelta m_DeltaTime     = 0.0f;
        float     m_LastFrameTime = 0.0f;

        bool m_Running;
        bool m_Minimized;

    private:
        // Application specific event callbacks
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:
        static Application* s_AppInstance;
    };
}
