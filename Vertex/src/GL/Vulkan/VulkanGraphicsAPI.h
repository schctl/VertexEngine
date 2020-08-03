#pragma once

#include "GL/GraphicsAPI.h"

namespace Vertex
{
    class VulkanGraphicsAPI : public GraphicsAPI
    {
    public:
        inline void Init() override {};

        void Clear() override {};
        void Clear(const glm::vec3& color) override {};
        void Clear(const glm::vec4& color) override {};

        std::string GetRendererInfo() const override {};

        virtual void DrawIndexed(Ref<VertexArray> vertex_array) override {};
    };
} // namespace Vertex
