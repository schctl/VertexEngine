#pragma once

#include "Core/Core.h"

#include "Core/Event/Event.h"

#include "GL/GraphicsContext.h"

#include "Core/Time/Time.h"

#include "Input.h"

namespace Vertex
{
    // utility class
    struct WindowProperties
    {
        bool         v_sync;
        const char*  title;
        unsigned int width, height;

        std::function<void(Event&)> event_callback;

        WindowProperties(const char* _title = "VertexEngine", unsigned int _width = 1280, unsigned int _height = 720,
                         bool _v_sync = true)
            : title(_title), width(_width), height(_height), v_sync(_v_sync)
        {
        }
    };

    class Window
    {
    public:
        virtual ~Window()                           = default;
        virtual void OnUpdate(TimeDelta delta_time) = 0;

        virtual void OnEvent(Event& event) = 0;

        virtual unsigned int GetWidth() const  = 0;
        virtual unsigned int GetHeight() const = 0;

        virtual void SetEventCallback(std::function<void(Event&)> func) = 0;

        virtual void SetVSync(bool conf) = 0;
        virtual bool IsVSync() const     = 0;

        virtual void* GetNativeWindow() const = 0;

        virtual bool ShouldClose() const = 0;

        virtual GraphicsContext& GetGraphicsContext() const = 0;

        static Window* Create(const WindowProperties properties = WindowProperties());
    };
}