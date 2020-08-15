#pragma once

#include "Core/Core.h"

namespace Vertex
{
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

    struct BufferElement
    {
        ShaderDataType type;
        size_t         offset;
        size_t         size;
        uint32_t       component_count;
        bool           normalized;

        BufferElement(ShaderDataType _type, bool _normalized = false)
            : type(_type), offset(0), size(GetSizeOfShaderDataType(_type)),
              component_count(GetComponentCountOfShaderDataType(_type)), normalized(_normalized)
        {
        }
    };

    class VX_PUBLIC_API BufferLayout
    {
    public:
        BufferLayout() {};

        BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements), m_Stride(0)
        {
            size_t offset = 0;
            for (auto& elem : m_Elements)
            {
                elem.offset = offset;
                offset += elem.size;
                m_Stride += elem.size;
            }
        }

        inline const size_t GetStride() const { return m_Stride; }

        inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

    private:
        std::vector<BufferElement> m_Elements;

        size_t m_Stride;

    public:
        // utility
        std::vector<BufferElement>::iterator       begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator       end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    };

    class Shader
    {
    public:
        virtual ~Shader() { }

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        static Shader* Create(std::vector<char>& vertex_src, std::vector<char>& fragment_src,
                              const BufferLayout vertex_layout);

        static std::vector<char> ReadSPIRVFromFile(const char* file_path);
    };
}