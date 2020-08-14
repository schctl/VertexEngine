#include "Shader.h"

#if defined(VX_RENDER_API_VULKAN)
    #include "Vulkan/VulkanShader.h"
#else
    #include "OpenGL/OpenGLShader.h"
// ... per rendering API
#endif

#include <fstream>

namespace Vertex
{
    // helper functions
    const size_t GetSizeOfShaderDataType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Bool:
            return 1;
        case ShaderDataType::Mat3:
            return 36;
        case ShaderDataType::Mat4:
            return 64;
        case ShaderDataType::Int:
            return 4;
        case ShaderDataType::Int2:
            return 8;
        case ShaderDataType::Int3:
            return 12;
        case ShaderDataType::Int4:
            return 16;
        case ShaderDataType::Float:
            return 4;
        case ShaderDataType::Float2:
            return 8;
        case ShaderDataType::Float3:
            return 12;
        case ShaderDataType::Float4:
            return 16;
        }

        CoreLogger::Error("Unknown shader data type, cancelling...");
        return 0;
    };

    const uint32_t GetComponentCountOfShaderDataType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Bool:
            return 1;
        case ShaderDataType::Mat3:
            return 9;
        case ShaderDataType::Mat4:
            return 16;
        case ShaderDataType::Int:
            return 1;
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Int4:
            return 4;
        case ShaderDataType::Float:
            return 1;
        case ShaderDataType::Float2:
            return 2;
        case ShaderDataType::Float3:
            return 3;
        case ShaderDataType::Float4:
            return 4;
        }

        CoreLogger::Error("Unknown shader data type, cancelling...");
        return 0;
    }

    std::vector<char> Shader::ReadSPIRVFromFile(const char* file_path)
    {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("Failed to open file!");

        size_t file_size = (size_t)file.tellg();

        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();

        return buffer;
    }

    // clang-format off

#if defined(VX_RENDER_API_VULKAN)

    Shader* Shader::Create(std::vector<char>& vertex_src, std::vector<char>& fragment_src)
    {
        return new VulkanShader(vertex_src, fragment_src);
    }

#else

    Shader* Shader::Create(std::vector<char>& vertex_src, std::vector<char>& fragment_src)
    {
        return new OpenGLShader(vertex_src, fragment_src);
    }

#endif

    // clang-format on
}