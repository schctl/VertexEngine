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

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
        glTextureStorage2D(m_ID, 1, GL_RGB8, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &m_ID); }

    void OpenGLTexture2D::Bind(uint32_t slot) const { glBindTextureUnit(slot, m_ID); }
} // namespace Vertex
