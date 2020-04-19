#pragma once

#include "Event.h"

namespace Vertex {

    class KeyEvent : Event
    {
    public:
        KeyEvent(int key)
            : m_Key(key)
        {
        }

        inline int GetKey() { return m_Key; }

        inline EventCategories GetEventCategory() override { return EventCategories::KeyEvent; }

    protected:
        int m_Key;
    };

    class KeyPressEvent : public KeyEvent
    {
    public:
        KeyPressEvent(int key, int repeat_count)
            : KeyEvent(key), m_RepeatCount(repeat_count)
        {
        }

        inline int GetRepeatCount() { return m_RepeatCount; }

        inline EventTypes  GetEventType() override { return EventTypes::KeyPress; }
        inline const char* GetEventName() override { return VX_GET_EVENT_TYPE_CHAR(EventTypes::KeyPress); }

        const char* GetDetails() override
        {
            return fmt::format("KeyPressEvent : Key<{0}> RepeatCount<{1}>", m_Key, m_RepeatCount).c_str();
        }

    private:
        int m_RepeatCount;
    };

    class KeyReleaseEvent : public KeyEvent
    {
    public:
        KeyReleaseEvent(int key)
            : KeyEvent(key)
        {
        }

        inline EventTypes  GetEventType() override { return EventTypes::KeyRelease; }
        inline const char* GetEventName() override { return VX_GET_EVENT_TYPE_CHAR(EventTypes::KeyRelease); }

        const char* GetDetails() override
        {
            return fmt::format("KeyReleaseEvent : Key<{0}>", m_Key).c_str();
        }
    };

}