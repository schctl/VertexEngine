#include "Application.h"

#if defined(VX_RENDER_API_OPENGL)
    #include "GL/OpenGL/OpenGLShader.h"
#endif

namespace Vertex
{

    Application* Application::s_AppInstance = nullptr;

    Application::Application()
        : m_Running(true),
          m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        VX_CORE_ASSERT((!s_AppInstance), "Application cannot be instantiated twice!");

        m_Window.reset(Window::Create());

        m_Window->SetEventCallback(VX_BIND_FUNC_1(Application::OnEvent));

        // --------------------------------------
        // ------------- Temporary --------------
        // ------ Should be in client side ------
        // --------------------------------------

#if defined(VX_RENDER_API_OPENGL)

        float vertices[21] = {
            -0.5f, -0.5f, 0.0f,   0.4f, 0.8f, 0.4f, 1.0f,
             0.5f, -0.5f, 0.0f,   0.4f, 0.8f, 0.4f, 1.0f,
            -0.5f,  0.5f, 0.0f,   0.4f, 0.8f, 0.4f, 1.0f
        };

        uint32_t indices[3] = { 0, 1, 2 };

        BufferLayout layout = { { ShaderDataType::Float3 }, { ShaderDataType::Float4 } };
          
        m_VertexArray.reset(VertexArray::Create());
        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices), layout));
    
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // --------------------------------------

        float vertices2[21] = {
             0.5f, -0.5f, 0.0f,   0.8f, 0.4f, 0.4f, 1.0f,
             0.5f,  0.5f, 0.0f,   0.8f, 0.4f, 0.4f, 1.0f,
            -0.5f,  0.5f, 0.0f,   0.8f, 0.4f, 0.4f, 1.0f
        };

        uint32_t indices2[3] = { 0, 1, 2 };

        BufferLayout layout2 = { { ShaderDataType::Float3 }, { ShaderDataType::Float4 } };

        m_VertexArray2.reset(VertexArray::Create());
        m_VertexBuffer2.reset(VertexBuffer::Create(vertices2, sizeof(vertices2), layout2));

        m_VertexArray2->AddVertexBuffer(m_VertexBuffer2);

        m_IndexBuffer2.reset(IndexBuffer::Create(indices2, sizeof(indices2)));

        m_VertexArray2->SetIndexBuffer(m_IndexBuffer2);

        const char* vertex_src = R"(
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

        const char* fragment_src = R"(
            #version 330 core

            in vec4 v_Color;

            out vec4 o_Color;

            void main()
            {
                o_Color = v_Color;
            }
        )";

        m_Shader.reset(Shader::Create(vertex_src, fragment_src));

        std::dynamic_pointer_cast<OpenGLShader>(m_Shader)->LoadUniform("u_ProjectionViewMatrix");

        // --------------------------------------
        // --------------------------------------
        // --------------------------------------

#endif // VX_RENDER_API_OPENGL

        s_AppInstance = this;

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        PopOverlay(m_ImGuiLayer);
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
        m_Window->GetGraphicsContext().SetRenderCallback(VX_BIND_FUNC_0(Application::Render));

        while (m_Running)
        {
            // ------------- Temporary --------------

#if defined(VX_RENDER_API_OPENGL)

            Renderer::Clear({0.1f, 0.1f, 0.1f, 1.0f});

            Renderer::BeginScene(m_Camera);

            Renderer::Submit(m_VertexArray, m_Shader);
            Renderer::Submit(m_VertexArray2, m_Shader);

            Renderer::EndScene();

#endif

            // --------------------------------------

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            // --------------- ImGui ----------------

            m_ImGuiLayer->Begin();

            for (Layer* layer : m_LayerStack)
                layer->OnImguiRender();
            m_Window->GetGraphicsContext().Render();

            // --------------------------------------

            m_Window->OnUpdate();
        }

        m_ImGuiLayer->OnDetach();

        return;
    }

    void Application::Render()
    {
        m_ImGuiLayer->End();
    }

    // Application specific event callbacks
    bool Application::OnWindowCloseEvent(Event& event)
    {
        m_Running = false;
        return true;
    }

}
