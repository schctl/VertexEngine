#include "Application.h"

namespace SandBox
{
    ExampleLayer::ExampleLayer(const char* name /* = "ExampleLayer" */)
        : Layer(name), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        m_CameraPosition = m_Camera.GetPosition();
        m_CameraRotation = m_Camera.GetRotation();

        // clang-format off

        float vertices[28] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        uint32_t indices[6] = {
            0, 1, 3,
            1, 2, 3 
        };

        // clang-format on

        Vertex::BufferLayout layout = { Vertex::ShaderDataType::Float3 };

        m_VertexArray.reset(Vertex::VertexArray::Create());
        m_VertexBuffer.reset(Vertex::VertexBuffer::Create(vertices, sizeof(vertices), layout));

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer.reset(Vertex::IndexBuffer::Create(indices, sizeof(indices)));

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // --------------------------------------

        // clang-format off

        float vertices2[21] = {
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        // clang-format on

        uint32_t indices2[3] = { 0, 1, 2 };

        Vertex::BufferLayout layout2 = { Vertex::ShaderDataType::Float3 };

        m_VertexArray2.reset(Vertex::VertexArray::Create());
        m_VertexBuffer2.reset(Vertex::VertexBuffer::Create(vertices2, sizeof(vertices2), layout2));

        m_VertexArray2->AddVertexBuffer(m_VertexBuffer2);

        m_IndexBuffer2.reset(Vertex::IndexBuffer::Create(indices2, sizeof(indices2)));

        m_VertexArray2->SetIndexBuffer(m_IndexBuffer2);

        // --------------------------------------

        const char* vertex_source = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_ProjectionViewMatrix;
            uniform mat4 u_Transform;

            void main()
            {
                gl_Position = u_ProjectionViewMatrix * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        const char* fragment_source = R"(
            #version 330 core

            uniform vec4 u_Color;

            out vec4 o_Color;

            void main()
            {
                o_Color = u_Color;
            }
        )";

        m_Shader.reset(Vertex::Shader::Create(vertex_source, fragment_source));

        Vertex::Logger::Info("Initialized test layer");
    }

    void ExampleLayer::OnUpdate(Vertex::TimeDelta delta_time)
    {
        if (Vertex::Input::IsKeyPressed(VX_KEY_W))
            m_CameraPosition.y -= m_CameraSpeed * delta_time.TotalSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_S))
            m_CameraPosition.y += m_CameraSpeed * delta_time.TotalSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_A))
            m_CameraPosition.x -= m_CameraSpeed * delta_time.TotalSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_D))
            m_CameraPosition.x += m_CameraSpeed * delta_time.TotalSeconds();

        if (Vertex::Input::IsKeyPressed(VX_KEY_Q))
            m_CameraRotation += m_CameraRotationSpeed * delta_time.TotalSeconds();
        if (Vertex::Input::IsKeyPressed(VX_KEY_E))
            m_CameraRotation -= m_CameraRotationSpeed * delta_time.TotalSeconds();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Vertex::Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });

        Vertex::Renderer::BeginScene(m_Camera);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        for (float x = 0.0f; x < 20.0f; x++)
        {
            for (float y = 0.0f; y < 20.0f; y++)
            {
                // clang-format off

                if (std::fmod(x, 2.0f) == 0)
                    (*std::dynamic_pointer_cast<Vertex::OpenGLShader>(m_Shader))["u_Color"] = glm::vec4(1.0f, 0.4f, 0.5f, 1.0f);
                else
                    (*std::dynamic_pointer_cast<Vertex::OpenGLShader>(m_Shader))["u_Color"] = glm::vec4(0.4f, 1.0f, 0.5f, 1.0f);

                Vertex::Renderer::Submit(m_VertexArray, m_Shader,
                                         glm::translate(
                                            glm::mat4(1.0f), glm::vec3(x * 0.11f, y * 0.11f, 0.0f)
                                         ) * scale);

                // clang-format on
            }
        }

        (*std::dynamic_pointer_cast<Vertex::OpenGLShader>(m_Shader))["u_Color"] = glm::vec4(0.5f, 0.4f, 1.0f, 1.0f);

        Vertex::Renderer::Submit(m_VertexArray2, m_Shader, glm::vec3(1.0f, 1.0f, 0.0f));

        Vertex::Renderer::EndScene();

        m_AvgFrameRate = (m_AvgFrameRate == 0) ? (1.0f / delta_time.TotalSeconds())
                                               : (m_AvgFrameRate + (1.0f / delta_time.TotalSeconds())) / 2;
    }

    void ExampleLayer::OnGUIRender(Vertex::TimeDelta delta_time)
    {
        ImGui::Begin("Renderer");
        ImGui::Text("%s", Vertex::Renderer::GetRendererInfo().c_str());
        ImGui::Text("\n %.2f", m_AvgFrameRate);
        ImGui::End();
    }
}

Vertex::Application* Vertex::CreateApplication()
{
    return new SandBox::SandBoxApp("SandBoxApp");
}