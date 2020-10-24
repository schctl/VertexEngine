#pragma once

#include "Core/Core.h"

#include "GL/GraphicsContext.h"

namespace Vertex
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* window);

        void SwapBuffers() override;

        inline void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) const override
        {
            glViewport(x, y, w, h);
        }

    private:
        GLFWwindow* m_WindowHandle;
    };
}
