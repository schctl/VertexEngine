#pragma once

#include "Application.h"

namespace Vertex
{
    extern Application* CreateApplication();
}

int main()
{
    auto app = Vertex::CreateApplication();
    app->Run();
    delete app;
}