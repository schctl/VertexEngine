#include "VertexArray.h"

#include "OpenGL/OpenGLVertexArray.h"
// ... per rendering api

namespace Vertex
{
    VertexArray* VertexArray::Create() { return new OpenGLVertexArray(); }
}