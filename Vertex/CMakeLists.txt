cmake_minimum_required(VERSION 3.16)

project(Vertex CXX)

file(GLOB VX_SOURCES
    src/Core/*.cpp
    src/Core/Event/*.cpp
    src/Core/Layer/*.cpp
    src/Core/Renderer/*.cpp
    src/Core/Time/*.cpp
    src/GL/*.cpp
    src/GUI/*.cpp
    src/Window/*.cpp
)

file(GLOB VX_HEADERS
    src/Core/*.h
    src/Core/Event/*.h
    src/Core/Layer/*.h
    src/Core/Renderer/*.h
    src/Core/Time/*.h
    src/GL/*.h
    src/GUI/*.h
    src/Window/*.h
    deps/glm/glm/*.hpp
)

file(GLOB VX_GRAPHICS_SOURCES src/GL/OpenGL/*.cpp)
file(GLOB VX_GRAPHICS_HEADERS src/GL/OpenGL/*.h)

set(IMGUI_IMPL_HEADERS
    deps/imgui/examples/imgui_impl_glfw.h
    deps/imgui/examples/imgui_impl_opengl3.h
)
set(IMGUI_IMPL_SOURCES
    deps/imgui/examples/imgui_impl_glfw.cpp
    deps/imgui/examples/imgui_impl_opengl3.cpp
)

add_subdirectory(deps/glad)
add_subdirectory(deps/imgui)

if (WIN32)
    file(GLOB_RECURSE VX_PLATFORM_HEADERS src/Window/Windows/*.h)
    file(GLOB_RECURSE VX_PLATFORM_SOURCES src/Window/Windows/*.cpp)
    
    find_package(OpenGL REQUIRED) # OpenGL
    set(GRAPHICS_LIB_INCLUDES ${OPENGL_INCLUDE_DIR} deps/glad/include)
    set(GRAPHICS_LIBS ${OPENGL_LIBRARIES})

    add_subdirectory(deps/glfw)

    set(PLATFORM_SPECIFIC_INCLUDES ${GRAPHICS_LIB_INCLUDES} deps/glfw/include ${GLFW_INCLUDE_DIRS})
    set(PLATFORM_SPECIFIC_LIBS ${GRAPHICS_LIBS} glfw)

elseif (UNIX AND NOT APPLE)
    file(GLOB_RECURSE VX_PLATFORM_HEADERS src/Window/Linux/*.h)
    file(GLOB_RECURSE VX_PLATFORM_SOURCES src/Window/Linux/*.cpp)

    find_package(OpenGL REQUIRED) # OpenGL
    set(GRAPHICS_LIB_INCLUDES ${OPENGL_INCLUDE_DIR})
    set(GRAPHICS_LIBS ${OPENGL_LIBRARIES})

    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARY glfw) # FindGLFW.cmake does not set this so we need to do it

    set(PLATFORM_SPECIFIC_INCLUDES ${GRAPHICS_LIB_INCLUDES} ${GLEW_INCLUDE_DIRS})
    set(PLATFORM_SPECIFIC_LIBS ${GRAPHICS_LIBS} ${GLFW_LIBRARY})

else ()
    message(FATAL_ERROR Unsupported platform)
endif ()

add_library(Vertex STATIC
    ${VX_HEADERS}
    ${VX_SOURCES}

    ${VX_GRAPHICS_HEADERS}
    ${VX_GRAPHICS_SOURCES}

    ${VX_PLATFORM_HEADERS}
    ${VX_PLATFORM_SOURCES}


    ${IMGUI_IMPL_HEADERS}
    ${IMGUI_IMPL_SOURCES}
)

target_compile_definitions(Vertex PUBLIC VX_RENDER_API_OPENGL)

# add the paths in PLATFORM_SPECIFIC_INCLUDES to Vertex's include directories
target_include_directories(Vertex PUBLIC
    src
    deps/spdlog/include
    deps/imgui
    deps/glm
    ${PLATFORM_SPECIFIC_INCLUDES}
)

# use the libs specified above in PLATFORM_SPECIFIC_LIBS and glad
target_link_libraries(Vertex PUBLIC
    ${PLATFORM_SPECIFIC_LIBS}
    glad 
    imgui
)

set_target_properties(Vertex PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(Vertex PROPERTIES PUBLIC_HEADER include/Vertex.h)

# this is going to be exported so we can use it in /CMakeLists.txt and /SandBox/CMakeLists.txt
set(VX_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

target_precompile_headers(Vertex PUBLIC src/Core/Core.h)

target_compile_definitions(Vertex PRIVATE
    VX_BUILD_SHARED
    IMGUI_IMPL_OPENGL_LOADER_GLAD=1
)

if ($ENV{VX_RELEASE})
    add_compile_definitions(NDEBUG)
endif ()

if ($ENV{CLION_IDE})
    target_compile_options(Vertex PUBLIC -include Core/Core.h)
endif ()