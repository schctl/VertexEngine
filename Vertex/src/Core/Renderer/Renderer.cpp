#include "Renderer.h"

#if defined(VX_RENDER_API_VULKAN)
#else
    #include "GL/OpenGL/OpenGLShader.h"
// .. per rendering api
#endif

namespace Vertex
{
    GraphicsAPI* Renderer::s_GraphicsAPI = GraphicsAPI::Create();
    Scene*       Renderer::s_Scene       = new Scene;

    void Renderer::BeginScene(Camera& camera) { s_Scene->ProjectionViewMatrix = camera.GetProjectionViewMatrix(); }

    void Renderer::Submit(const Ref<VertexArray>& vertex_array, const Ref<Shader>& shader, const glm::mat4& transform)
    {
        shader->Bind();

#if defined(VX_RENDER_API_VULKAN)

#else

        (*std::dynamic_pointer_cast<OpenGLShader>(shader))["u_ProjectionViewMatrix"] = s_Scene->ProjectionViewMatrix;

        (*std::dynamic_pointer_cast<OpenGLShader>(shader))["u_Transform"] = transform;

#endif

        vertex_array->Bind();

        s_GraphicsAPI->DrawIndexed(vertex_array);
    }

    void Renderer::Submit(const Ref<VertexArray>& vertex_array, const Ref<Shader>& shader, const glm::vec3& position)
    {
        shader->Bind();

#if defined(VX_RENDER_API_VULKAN)

#else

        (*std::dynamic_pointer_cast<OpenGLShader>(shader))["u_ProjectionViewMatrix"] = s_Scene->ProjectionViewMatrix;

        (*std::dynamic_pointer_cast<OpenGLShader>(shader))["u_Transform"] = glm::translate(glm::mat4(1.0f), position);

#endif

        vertex_array->Bind();

        s_GraphicsAPI->DrawIndexed(vertex_array);
    }

    void Renderer::EndScene() { }

    RenderAPI Renderer::GetAPI()
    {
#if defined(VX_RENDER_API_OPENGL)

        return RenderAPI::OpenGL;

#elif defined(VX_RENDER_API_VULKAN)

        return RenderAPI::Vulkan;
#endif
    }
}
