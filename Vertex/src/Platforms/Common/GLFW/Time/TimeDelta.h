#pragma once

#include "Core/Core.h"

namespace Vertex
{

    enum class TimeUnit
    {
        Seconds,
        Milliseconds,
        Microseconds,
        Nanoseconds
    };

    struct TimeDelta
    {
    public:
        TimeDelta(float seconds) : m_Seconds(seconds) { }

        const float GetSeconds() { return m_Seconds; }
        const float GetMilliseconds() { return m_Seconds * 1000.0f; }

    private:
        float m_Seconds;
    };

} // namespace Vertex
