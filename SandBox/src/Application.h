#pragma once

#include <Vertex.h>

#include <memory>

namespace SandBox
{
    class ExampleLayer : public Vertex::Layer
    {
    public:
        ExampleLayer(const char* name = "Example Layer");

        void OnUpdate(Vertex::TimeDelta delta_time) override;
        void OnGUIRender(Vertex::TimeDelta delta_time) override;

    private:
        std::shared_ptr<Vertex::Shader> m_Shader;

        std::shared_ptr<Vertex::VertexArray>  m_VertexArray;
        std::shared_ptr<Vertex::VertexBuffer> m_VertexBuffer;
        std::shared_ptr<Vertex::IndexBuffer>  m_IndexBuffer;

        std::shared_ptr<Vertex::VertexArray>  m_VertexArray2;
        std::shared_ptr<Vertex::VertexBuffer> m_VertexBuffer2;
        std::shared_ptr<Vertex::IndexBuffer>  m_IndexBuffer2;

        Vertex::OrthographicCamera2D m_Camera;

        glm::vec3 m_CameraPosition;
        float     m_CameraRotation;

        const float m_CameraSpeed         = 0.5f;
        const float m_CameraRotationSpeed = 30.0f;

        float m_AvgFrameRate;
    };

    class SandBoxApp : public Vertex::Application
    {
    public:
        SandBoxApp(const char* name) : Vertex::Application(name)
        {
            m_ExampleLayer = new ExampleLayer();
            PushLayer(m_ExampleLayer);
        }

        ~SandBoxApp() { delete m_ExampleLayer; }

    private:
        ExampleLayer* m_ExampleLayer;
    };
}
