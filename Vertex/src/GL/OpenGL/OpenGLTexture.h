#pragma once

#include "Core/Core.h"

#include "GL/Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Vertex
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const char* path);
        ~OpenGLTexture2D();

        inline uint32_t GetWidth() const override { return m_Width; };
        inline uint32_t GetHeight() const override { return m_Height; };

        void Bind(uint32_t slot = 0) const override;

    private:
        const char* m_Path; // debugging

        uint32_t m_Width;
        uint32_t m_Height;

        uint32_t m_ID;
    };
} // namespace Vertex
