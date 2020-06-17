#include "OpenGLTexture.h"

namespace Vertex
{
    OpenGLTexture2D::OpenGLTexture2D(const char* path) : m_Path(path)
    {
        int _w, _h, _c;

        stbi_uc* data = stbi_load(path, &_w, &_h, &_c, 0);

        VX_CORE_ASSERT(data, "Failed to load texture at {}", path);

        m_Width  = _w;
        m_Height = _h;

        uint32_t internal_format = 0, /* internal format represents the format the texture must be interpreted in */
            data_format          = 0; /* data format represents the actual format of the texture */

        if (_c == 4)
        {
            internal_format = GL_RGBA8;
            data_format     = GL_RGBA;
        }
        else if (_c == 3)
        {
            internal_format = GL_RGB8;
            data_format     = GL_RGB;
        }
        else
        {
            VX_CORE_ASSERT(false, "Unsupported texture format.");
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
        glTextureStorage2D(m_ID, 1, internal_format, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, data_format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &m_ID); }

    void OpenGLTexture2D::Bind(uint32_t slot) const { glBindTextureUnit(slot, m_ID); }
} // namespace Vertex
