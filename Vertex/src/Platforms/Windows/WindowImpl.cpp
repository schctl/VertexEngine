#include "WindowImpl.h"

#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"

namespace Vertex {

    static bool s_GLFW_Initialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        Logger::GetCoreLogger()->error("GLFW error {0} : {1}", error, description);
    }

    namespace GLFWInputCallbacks
    {
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowProperties* properties = (WindowProperties*)glfwGetWindowUserPointer(window);

            if (properties == nullptr)
            {
                Logger::GetCoreLogger()->error("Null window properties");
                return;
            }

            if (action == GLFW_PRESS)
            {
                KeyPressEvent e(key, 1);
                properties->event_callback(e);
            }
            else if (action == GLFW_RELEASE)
            {
                KeyReleaseEvent e(key);
                properties->event_callback(e);
            }
        }

        static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
        {
            WindowProperties* properties = (WindowProperties*)glfwGetWindowUserPointer(window);

            if (properties == nullptr)
            {
                Logger::GetCoreLogger()->error("Null window properties");
                return;
            }

            MouseMoveEvent e((int)xpos, (int)ypos);
            properties->event_callback(e);
        }

        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            WindowProperties* properties = (WindowProperties*)glfwGetWindowUserPointer(window);

            if (properties == nullptr)
            {
                Logger::GetCoreLogger()->error("Null window properties");
                return;
            }

            if (action == GLFW_PRESS)
            {
                MouseClickEvent e(button);
                properties->event_callback(e);
            }
            else if (action == GLFW_RELEASE)
            {
                MouseReleaseEvent e(button);
                properties->event_callback(e);
            }
        }

        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowProperties* properties = (WindowProperties*)glfwGetWindowUserPointer(window);

            if (properties == nullptr)
            {
                Logger::GetCoreLogger()->error("Null window properties");
                return;
            }

            MouseScrollEvent e(xoffset, yoffset);
            properties->event_callback(e);
        }
    }

    WindowImpl::WindowImpl(WindowProperties properties)
        : m_Data(properties)
    {
        Logger::GetCoreLogger()->info("Creating window {}", m_Data.title);

        if (!s_GLFW_Initialized)
        {
            int success = glfwInit();

            if (!success)
                Logger::GetCoreLogger()->error("Could not initialize GLFW");

            s_GLFW_Initialized = true;
        }

        m_Window = glfwCreateWindow((int)m_Data.width, (int)m_Data.height, m_Data.title, nullptr, nullptr);

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            Logger::GetCoreLogger()->error("Could not initialize glad.");

        glfwSetWindowUserPointer(m_Window, &m_Data);

        // set up glfw input callbacks
        glfwSetKeyCallback(m_Window, GLFWInputCallbacks::KeyCallback);
        glfwSetCursorPosCallback(m_Window, GLFWInputCallbacks::CursorPositionCallback);
        glfwSetMouseButtonCallback(m_Window, GLFWInputCallbacks::MouseButtonCallback);
        glfwSetScrollCallback(m_Window, GLFWInputCallbacks::ScrollCallback);
    }

    WindowImpl::~WindowImpl()
    {
        ShutDown();
    }

    void WindowImpl::ShutDown()
    {
        glfwDestroyWindow(m_Window); // we may want to shutdown before the window's scope is over
    }

    void WindowImpl::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

}