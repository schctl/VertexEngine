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
        glBindTexture(GL_TEXTURE_2D, m_ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &m_ID); }

    void OpenGLTexture2D::Bind(uint32_t slot) const { glBindTextureUnit(slot, m_ID); }
} // namespace Vertex
