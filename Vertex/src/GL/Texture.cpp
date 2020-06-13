#include "Texture.h"

#include "OpenGL/OpenGLTexture.h"

namespace Vertex
{
    Ref<Texture2D> Texture2D::Create(const char* path)
    {
        return std::make_shared<OpenGLTexture2D>(OpenGLTexture2D(path));
    }
} // namespace Vertex
