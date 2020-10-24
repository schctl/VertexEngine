#include "Texture.h"

#if defined(VX_RENDER_API_VULKAN)
#else
    #include "OpenGL/OpenGLTexture.h"
// ... per rendering API
#endif

namespace Vertex
{
    // clang-format off

#if defined (VX_RENDER_API_VULKAN)

    Texture2D* Texture2D::Create(const char* path)
    {
        return nullptr;
    }

#else

    Texture2D* Texture2D::Create(const char* path)
    {
        return new OpenGLTexture2D(path);
    }

#endif

    // clang-format on
} // namespace Vertex
