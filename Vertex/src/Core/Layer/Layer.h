#pragma once

#include "Core/Core.h"
#include "Core/Event/Event.h"

#include "Platforms/Common/GLFW/Time/Time.h"

namespace Vertex
{

    class VX_PUBLIC_API Layer
    {
    public:
        Layer(const char* name = "layer");
        virtual ~Layer();

        virtual void OnAttach() { }
        virtual void OnDetach() { }

        virtual void OnUpdate(TimeDelta delta_time) { }
        virtual void OnEvent(Event& event) { }

        virtual void OnGUIRender() { }

        inline const char* GetName() { return m_Name; }

    protected:
        const char* m_Name; // this is supposed to be used for debugging only
    };

}