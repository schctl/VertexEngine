#pragma once

#include "Core/Core.h"

#include "GL/GraphicsAPI.h"
#include "GL/Shader.h"
#include "GL/Texture.h"
#include "GL/VertexArray.h"

#include "Camera.h"

namespace Vertex
{
    enum class RenderAPI
    {
        OpenGL,
        Vulkan
    };

    struct Scene
    {
        glm::mat4 ProjectionViewMatrix;
    };

    class VX_PUBLIC_API Renderer
    {
    public:
        inline static void Init() { s_GraphicsAPI->Init(); }

        inline static void Clear() { s_GraphicsAPI->Clear(); }
        inline static void Clear(const glm::vec3& color) { s_GraphicsAPI->Clear(color); }
        inline static void Clear(const glm::vec4& color) { s_GraphicsAPI->Clear(color); }

        // inline static void NotifyResize(uint32_t width, uint32_t height)
        // {
        //     s_GraphicsAPI->SetViewport(0, 0, width, height);
        // }

        static RenderAPI   GetAPI();
        static std::string GetRendererInfo() { return s_GraphicsAPI->GetRendererInfo(); }

    public:
        static void BeginScene(Camera& camera);

        static void Submit(const Ref<VertexArray>& vertex_array, const Ref<Shader>& shader,
                           const glm::mat4& transform = glm::mat4(1.0f));

        static void Submit(const Ref<VertexArray>& vertex_array, const Ref<Shader>& shader, const glm::vec3& position);

        static void EndScene();

    private:
        static GraphicsAPI* s_GraphicsAPI;
        static Scene*       s_Scene;
    };
}
