#include <Vertex.h>

class TestLayer : public Vertex::Layer
{
public:
    TestLayer(const char* name = "TestLayer") : Layer(name) { Vertex::ClientLogger::Info("Initialized test layer"); }

    void OnUpdate() override
    {
        /*
        m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
        m_Camera.SetRotation(45.0f);

        Renderer::BeginScene(m_Camera);

        Renderer::Submit(m_VertexArray, m_Shader);
        Renderer::Submit(m_VertexArray2, m_Shader);

        Renderer::EndScene();
        */
    }

    void OnEvent(Vertex::Event& event) override { }

    void OnGUIRender() override { Vertex::ImGuiLayer::ShowRendererInfo(); }
};

class SandBoxApp : public Vertex::Application
{
public:
    SandBoxApp() { PushLayer(new TestLayer()); }

    ~SandBoxApp() { }
};

Vertex::Application* Vertex::CreateApplication() { return new SandBoxApp(); }