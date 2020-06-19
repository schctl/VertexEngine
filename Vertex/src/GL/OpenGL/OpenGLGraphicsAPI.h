#pragma once

#include "GL/GraphicsAPI.h"

namespace Vertex
{
    class OpenGLGraphicsAPI : public GraphicsAPI
    {
    public:
        inline void Init() override
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        inline void Clear() override
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // for now
        }

        inline void Clear(const glm::vec3& color) override
        {
            glClearColor(color.r, color.g, color.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        inline void Clear(const glm::vec4& color) override
        {
            glClearColor(color.r, color.g, color.b, color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        inline void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) const override
        {
            glViewport(x, y, w, h);
        }

        inline std::string GetRendererInfo() const override
        {
            // clang-format off

            return fmt::format("Rendering API: OpenGL {0}\nRenderer: {1}\nVendored by: {2}",
                                glGetString ( GL_VERSION  ),
                                glGetString ( GL_RENDERER ),
                                glGetString (  GL_VENDOR  )
            );

            // clang-format on
        }

        inline void DrawIndexed(Ref<VertexArray> vertex_array) override
        {
            // clang-format off

            glDrawElements(GL_TRIANGLES, vertex_array->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr); // for now

            // clang-format on
        }
    };
}