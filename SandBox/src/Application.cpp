#include "Application.h"

namespace SandBox
{

    ExampleLayer::ExampleLayer(const char* name /* = "ExampleLayer" */)
        : Layer(name), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        // clang-format off

        float vertices[21] = {
            -0.5f, -0.5f, 0.0f,  0.4f, 0.8f, 0.4f, 1.0f,
             0.5f, -0.5f, 0.0f,  0.4f, 0.8f, 0.4f, 1.0f,
            -0.5f,  0.5f, 0.0f,  0.4f, 0.8f, 0.4f, 1.0f
        };

        // clang-format on

        uint32_t indices[3] = { 0, 1, 2 };

        Vertex::BufferLayout layout = { { Vertex::ShaderDataType::Float3 }, { Vertex::ShaderDataType::Float4 } };

        m_VertexArray.reset(Vertex::VertexArray::Create());
        m_VertexBuffer.reset(Vertex::VertexBuffer::Create(vertices, sizeof(vertices), layout));

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer.reset(Vertex::IndexBuffer::Create(indices, sizeof(indices)));

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // --------------------------------------

        // clang-format off

        float vertices2[21] = {
             0.5f, -0.5f, 0.0f,  0.8f, 0.4f, 0.4f, 1.0f,
             0.5f,  0.5f, 0.0f,  0.8f, 0.4f, 0.4f, 1.0f, 
            -0.5f,  0.5f, 0.0f,  0.8f, 0.4f, 0.4f, 1.0f
        };

        // clang-format on

        uint32_t indices2[3] = { 0, 1, 2 };

        Vertex::BufferLayout layout2 = { { Vertex::ShaderDataType::Float3 }, { Vertex::ShaderDataType::Float4 } };

        m_VertexArray2.reset(Vertex::VertexArray::Create());
        m_VertexBuffer2.reset(Vertex::VertexBuffer::Create(vertices2, sizeof(vertices2), layout2));

        m_VertexArray2->AddVertexBuffer(m_VertexBuffer2);

        m_IndexBuffer2.reset(Vertex::IndexBuffer::Create(indices2, sizeof(indices2)));

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

        m_Shader.reset(Vertex::Shader::Create(vertex_source, fragment_source));

        // for now
        std::dynamic_pointer_cast<Vertex::OpenGLShader>(m_Shader)->LoadUniform("u_ProjectionViewMatrix");

        Vertex::ClientLogger::Info("Initialized test layer");
    }

    void ExampleLayer::OnUpdate()
    {
        m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
        m_Camera.SetRotation(45.0f);

        Vertex::Renderer::BeginScene(m_Camera);

        Vertex::Renderer::Submit(m_VertexArray, m_Shader);
        Vertex::Renderer::Submit(m_VertexArray2, m_Shader);

        Vertex::Renderer::EndScene();
    }

    void ExampleLayer::OnEvent(Vertex::Event& event)
    {
        Vertex::EventHandler handler(event);

        handler.Dispatch<Vertex::EventTypes::KeyPress, Vertex::KeyPressEvent>(
            VX_BIND_FUNC_1(ExampleLayer::OnKeyPressEvent));
    }

    bool ExampleLayer::OnKeyPressEvent(Vertex::KeyPressEvent& event) { return false; }
}

Vertex::Application* Vertex::CreateApplication()
{
    return new SandBox::SandBoxApp();
}