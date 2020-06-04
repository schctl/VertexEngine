#pragma once

#include "Core/Core.h"

#include "TimeDelta.h"

namespace Vertex
{

    class VX_PUBLIC_API Time
    {
    public:
        inline static float GetTime() { return glfwGetTime(); }
    };

}