#pragma once

#include "Core/Core.h"

#include "GL/VertexArray.h"

namespace Vertex
{
    enum class GraphicsAPIAction
    {
        Clear,
        DrawIndexed
    };

    class GraphicsAPI
    {
    public:
        virtual void Init() = 0;

        virtual void Clear()                       = 0;
        virtual void Clear(const glm::vec3& color) = 0;
        virtual void Clear(const glm::vec4& color) = 0;

        virtual std::string GetRendererInfo() const = 0;

        virtual void DrawIndexed(Ref<VertexArray> vertex_array) = 0;

        static GraphicsAPI* Create();
    };
}