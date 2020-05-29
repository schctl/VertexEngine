#include "Application.h"

namespace SandBox
{

    ExampleLayer::ExampleLayer(const char* name /* = "ExampleLayer" */)
        : Layer(name), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        m_CameraPosition = m_Camera.GetPosition();
        m_CameraRotation = m_Camera.GetRotation();

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

    void ExampleLayer::OnUpdate(Vertex::TimeDelta delta_time)
    {
        if (Vertex::Input::IsKeyPressed(VX_KEY_W))
            m_CameraPosition.y -= m_CameraSpeed * delta_time.GetSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_S))
            m_CameraPosition.y += m_CameraSpeed * delta_time.GetSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_A))
            m_CameraPosition.x -= m_CameraSpeed * delta_time.GetSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_D))
            m_CameraPosition.x += m_CameraSpeed * delta_time.GetSeconds();

        if (Vertex::Input::IsKeyPressed(VX_KEY_Q))
            m_CameraRotation += m_CameraRotationSpeed * delta_time.GetSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_E))
            m_CameraRotation -= m_CameraRotationSpeed * delta_time.GetSeconds();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Vertex::Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });

        Vertex::Renderer::BeginScene(m_Camera);

        Vertex::Renderer::Submit(m_VertexArray, m_Shader);
        Vertex::Renderer::Submit(m_VertexArray2, m_Shader);

        Vertex::Renderer::EndScene();
    }

    void ExampleLayer::OnEvent(Vertex::Event& event) { }

    bool ExampleLayer::OnKeyPressEvent(Vertex::KeyPressEvent& event) { return false; }
}

Vertex::Application* Vertex::CreateApplication()
{
    return new SandBox::SandBoxApp();
}