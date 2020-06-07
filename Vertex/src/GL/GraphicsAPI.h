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
        virtual void Clear()                       = 0;
        virtual void Clear(const glm::vec3& color) = 0;
        virtual void Clear(const glm::vec4& color) = 0;

        /**
         * x, y - lower left corner of the viewport
         * w, h - width and height of the viewport
         */
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) const = 0;

        virtual std::string GetRendererInfo() const = 0;

        virtual void DrawIndexed(std::shared_ptr<VertexArray> vertex_array) = 0;

        static GraphicsAPI* Create();
    };
}