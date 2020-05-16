#pragma once

#include <Vertex.h>

#include <memory>

namespace SandBox
{

    class ExampleLayer : public Vertex::Layer
    {
    public:
        ExampleLayer(const char* name = "Example Layer");

        void OnUpdate() override;
        void OnEvent(Vertex::Event& event) override;
        void OnGUIRender() override { Vertex::ImGuiLayer::ShowRendererInfo(); }

    private:
        std::shared_ptr<Vertex::Shader> m_Shader;

        std::shared_ptr<Vertex::VertexArray>  m_VertexArray;
        std::shared_ptr<Vertex::VertexBuffer> m_VertexBuffer;
        std::shared_ptr<Vertex::IndexBuffer>  m_IndexBuffer;

        std::shared_ptr<Vertex::VertexArray>  m_VertexArray2;
        std::shared_ptr<Vertex::VertexBuffer> m_VertexBuffer2;
        std::shared_ptr<Vertex::IndexBuffer>  m_IndexBuffer2;

        Vertex::OrthographicCamera2D m_Camera;

    private:
        // event callbacks
        bool OnKeyPressEvent(Vertex::KeyPressEvent& event);
    };

    class SandBoxApp : public Vertex::Application
    {
    public:
        SandBoxApp() { PushLayer(new ExampleLayer()); }
        ~SandBoxApp() { }
    };

}
