#include "Application.h"

#include "GL/OpenGL/OpenGLShader.h"

namespace Vertex
{

    Application* Application::s_AppInstance = nullptr;

    Application::Application() : m_Running(true), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        VX_CORE_ASSERT((!s_AppInstance), "Application cannot be instantiated twice!");

        m_Window.reset(Window::Create());

        m_Window->SetEventCallback(VX_BIND_FUNC_1(Application::OnEvent));

        // --------------------------------------
        // ------------- Temporary --------------
        // ------ Should be in client side ------
        // --------------------------------------

        float vertices[21] = { -0.5f, -0.5f, 0.0f, 0.4f, 0.8f, 0.4f, 1.0f, 0.5f, -0.5f, 0.0f, 0.4f, 0.8f, 0.4f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.4f, 0.8f, 0.4f, 1.0f };

        uint32_t indices[3] = { 0, 1, 2 };

        BufferLayout layout = { { ShaderDataType::Float3 }, { ShaderDataType::Float4 } };

        m_VertexArray.reset(VertexArray::Create());
        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices), layout));

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // --------------------------------------

        float vertices2[21] = { 0.5f, -0.5f, 0.0f, 0.8f, 0.4f, 0.4f, 1.0f, 0.5f, 0.5f, 0.0f, 0.8f, 0.4f, 0.4f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.8f, 0.4f, 0.4f, 1.0f };

        uint32_t indices2[3] = { 0, 1, 2 };

        BufferLayout layout2 = { { ShaderDataType::Float3 }, { ShaderDataType::Float4 } };

        m_VertexArray2.reset(VertexArray::Create());
        m_VertexBuffer2.reset(VertexBuffer::Create(vertices2, sizeof(vertices2), layout2));

        m_VertexArray2->AddVertexBuffer(m_VertexBuffer2);

        m_IndexBuffer2.reset(IndexBuffer::Create(indices2, sizeof(indices2)));

        m_VertexArray2->SetIndexBuffer(m_IndexBuffer2);

        // --------------------------------------

        const char* vertex_source = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ProjectionViewMatrix;

            out vec4 v_Color;

            void main()
            {
                gl_Position = u_ProjectionViewMatrix * vec4(a_Position, 1.0);
                v_Color = a_Color;
            }
        )";

        const char* fragment_source = R"(
            #version 330 core

            in vec4 v_Color;

            out vec4 o_Color;

            void main()
            {
                o_Color = v_Color;
            }
        )";

        m_Shader.reset(Shader::Create(vertex_source, fragment_source));

        std::dynamic_pointer_cast<OpenGLShader>(m_Shader)->LoadUniform("u_ProjectionViewMatrix");

        // --------------------------------------
        // --------------------------------------
        // --------------------------------------

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
        }
    }

    void Application::Run()
    {
        while (m_Running)
        {
            Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            m_ImGuiLayer->Begin();

            for (Layer* layer : m_LayerStack)
                layer->OnGUIRender();

            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    // Application specific event callbacks
    bool Application::OnWindowCloseEvent(Event& event)
    {
        m_Running = false;
        return true;
    }

}
