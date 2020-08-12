#pragma once

#include "Core/Core.h"

namespace Vertex
{
    // Make definite shader types so the API is more consistent
    enum class ShaderDataType
    {
        Bool,
        Mat3,
        Mat4,
        Int,
        Int2,
        Int3,
        Int4,
        Float,
        Float2,
        Float3,
        Float4
    };

    // helper functions
    const size_t   GetSizeOfShaderDataType(ShaderDataType type);
    const uint32_t GetComponentCountOfShaderDataType(ShaderDataType type);

    class Shader
    {
    public:
        virtual ~Shader() { }

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        static Shader* Create(std::vector<char> vertex_src, std::vector<char> fragment_src);

        static std::vector<char> ReadSPIRVFromFile(const char* file_path);
    };
}