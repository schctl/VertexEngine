#pragma once

#include <imgui.h>

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "Core/Core.h"

#include "Core/Layer/Layer.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/WindowEvent.h"

namespace Vertex
{
    class VX_PUBLIC_API GUILayer : public Layer
    {
    public:
        GUILayer();
        ~GUILayer();

        void OnAttach() override;
        void OnDetach() override;

        void Begin();
        void End();

        static void ShowDockSpace(bool* p_open);
        static void ShowRendererInfo(bool* p_open);

    private:
        float m_Time;
    };
}