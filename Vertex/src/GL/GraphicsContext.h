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

        static GraphicsContext* Create(GLFWwindow* window_handle);
    };
}