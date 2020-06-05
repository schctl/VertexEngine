#pragma once

#include "Core/Core.h"

namespace Vertex
{
    enum class EventCategories
    {
        WindowEvent,
        KeyEvent,
        MouseEvent
    };

    enum class EventTypes
    {
        WindowResize,
        WindowClose,
        WindowGainedFocus,
        WindowLostFocus,
        WindowCursorEntered,
        WindowCursorLeft,

        KeyPress,
        KeyRelease,
        KeyCharInput,

        MouseClick,
        MouseRelease,
        MouseScroll,
        MouseMove
    };

    class VX_PUBLIC_API Event
    {
        friend class EventHandler;

    public:
        virtual EventCategories GetEventCategory() = 0;
        virtual EventTypes      GetEventType()     = 0;
        virtual const char*     GetEventName()     = 0;
        virtual std::string     GetDetails()       = 0;

        inline bool IsHandled() { return m_Handled; }

    protected:
        bool m_Handled = false;
    };

    class EventHandler
    {
    public:
        EventHandler(Event& event) : m_Event(event) { }

        template <typename EventTypeName, typename F>
        void Dispatch(F&& func)
        {
            try
            {
                m_Event.m_Handled = func(dynamic_cast<EventTypeName&>(m_Event));
            }
            catch (...)
            {
            }
        }

    private:
        Event& m_Event;
    };
}