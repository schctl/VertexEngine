#pragma once

#include "Core/Core.h"

namespace Vertex
{
    class GraphicsContext
    {
        // // the this pointer from GraphicsContext::Render
        // typedef std::function<void(GraphicsContext*)> PrepareRenderFunc;
        // typedef std::function<void(GraphicsContext*)> RenderFunc;

    public:
        virtual ~GraphicsContext() = default;

        virtual void SwapBuffers() = 0;

        /**
         * x, y - lower left corner of the viewport
         * w, h - width and height of the viewport
         */
        virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) const = 0;

        static GraphicsContext* Create(GLFWwindow* window_handle);
    };
}