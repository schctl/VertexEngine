#include "GraphicsAPI.h"

#include "OpenGL/OpenGLGraphicsAPI.h"
// ... per rendering API

namespace Vertex
{

    GraphicsAPI* GraphicsAPI::Create()
    {
#if defined(VX_RENDER_API_OPENGL)
        return new OpenGLGraphicsAPI();
#elif defined(VX_RENDER_API_VULKAN)
        return new VulkanGraphicsAPI();
#else
        return nullptr; // for now
#endif
    }
    template <GraphicsAPIAction action, typename... GraphicsAPIActionArgument>
    void GraphicsAPI::Queue(GraphicsAPIActionArgument... args)
    {
        if (action == GraphicsAPIAction::DrawIndexed)
        {
            DrawIndexed(std::get<0>(std::forward_as_tuple(args...)));
        }
    }

    template void GraphicsAPI::Queue<DrawIndexed, std::shared_ptr<VertexArray>>(
        std::shared_ptr<VertexArray> vertex_array);
}