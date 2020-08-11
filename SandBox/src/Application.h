#pragma once

#include <Vertex.h>

#include <memory>

namespace SandBox
{
#if defined(VX_RENDER_API_VULKAN)
    class ExampleLayer : public Vertex::Layer
    {
    public:
        ExampleLayer(const char* name = "Example Layer");

        void OnUpdate(const Vertex::TimeDelta delta_time) override;
        void OnGUIUpdate(const Vertex::TimeDelta delta_time) override {};

    private:
        Vertex::Ref<Vertex::VertexBuffer> m_VertexBuffer;
        Vertex::Ref<Vertex::IndexBuffer>  m_IndexBuffer;
    };

#else
    class ExampleLayer : public Vertex::Layer
    {
    public:
        ExampleLayer(const char* name = "Example Layer");

        void OnUpdate(const Vertex::TimeDelta delta_time) override;
        void OnGUIUpdate(const Vertex::TimeDelta delta_time) override;

    private:
        Vertex::Ref<Vertex::Shader> m_Shader;

        Vertex::Ref<Vertex::VertexArray>  m_VertexArray;
        Vertex::Ref<Vertex::VertexBuffer> m_VertexBuffer;
        Vertex::Ref<Vertex::IndexBuffer>  m_IndexBuffer;

        Vertex::Ref<Vertex::Texture2D> m_ArchTexture;
        Vertex::Ref<Vertex::Texture2D> m_VETexture;

        Vertex::OrthographicCamera2D m_Camera;

        glm::vec3 m_CameraPosition;
        float     m_CameraRotation;

        const float m_CameraSpeed         = 0.5f;
        const float m_CameraRotationSpeed = 30.0f;

        float m_AvgFrameRate;
    };

#endif

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
