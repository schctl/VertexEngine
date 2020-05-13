#include "GraphicsAPI.h"

#include "OpenGL/OpenGLGraphicsAPI.h"
// ... per rendering API

namespace Vertex
{

    GraphicsAPI* GraphicsAPI::Create() { return new OpenGLGraphicsAPI(); }

}