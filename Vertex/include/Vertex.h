/**
 *                  Copyright 2020 schctl
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * Usage
 * -----
 * **Do not** define a `main` function in any application that uses Vertex.
 * Vertex provides an entry point via the `CreateApplication` function.
 * Subclass `Vertex::Application` and return a heap allocated object of that class
 * by defining `Vertex::CreateApplication` in your project.
 */

#pragma once

#include "Core/Core.h"
// -------------------------------------
// ---- Main function defined here -----
#include "Core/EntryPoint.h"
// -------------------------------------
#include "Core/Application.h"

#include "Core/Event/Event.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/WindowEvent.h"

#include "Core/Input.h"
#include "Platforms/Common/GLFW/Time/Time.h"

#include "Core/Logger.h"

#include <imgui.h>

#include "ImGui/ImGuiLayer.h"

#include "Renderer/Renderer.h"

#include "GL/OpenGL/OpenGLShader.h" // very temporary