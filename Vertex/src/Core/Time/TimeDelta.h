#pragma once

#include "Core/Core.h"

namespace Vertex
{
    struct TimeDelta
    {
    public:
        TimeDelta(float seconds) : m_Seconds(seconds) { }

        const float TotalSeconds() { return m_Seconds; }

    private:
        float m_Seconds;
    };
} // namespace Vertex
