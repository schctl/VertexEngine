#pragma once

#include "Core/Core.h"

#include "Shader.h"

namespace Vertex
{
    // -----------------------------------
    // ---------- Buffer Layout ----------
    // -----------------------------------

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

    class Buffer
    {
    public:
        virtual ~Buffer() { }

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;
    };

    // -----------------------------------
    // ---------- Vertex Buffer ----------
    // -----------------------------------

    class VX_PUBLIC_API VertexBuffer : public Buffer
    {
    public:
        virtual ~VertexBuffer() { }

        virtual const BufferLayout& GetLayout() const = 0;

        static VertexBuffer* Create(float* vertices, size_t size, const BufferLayout& layout);
    };

    // ----------------------------------
    // ---------- Index Buffer ----------
    // ----------------------------------

    class VX_PUBLIC_API IndexBuffer : public Buffer
    {
    public:
        virtual ~IndexBuffer() { }

        virtual uint32_t GetCount() const = 0;

        static IndexBuffer* Create(uint32_t* indices, size_t size);
    };

    // ----------------------------------
    // --------- Uniform Buffer ---------
    // ----------------------------------

    class VX_PUBLIC_API UniformBuffer : public Buffer
    {
    public:
        virtual ~UniformBuffer() {};

        virtual void SetValue(bool value, uint32_t offset)       = 0;
        virtual void SetValue(glm::mat3 value, uint32_t offset)  = 0;
        virtual void SetValue(glm::mat4 value, uint32_t offset)  = 0;
        virtual void SetValue(int value, uint32_t offset)        = 0;
        virtual void SetValue(glm::ivec2 value, uint32_t offset) = 0;
        virtual void SetValue(glm::ivec3 value, uint32_t offset) = 0;
        virtual void SetValue(glm::ivec4 value, uint32_t offset) = 0;
        virtual void SetValue(float value, uint32_t offset)      = 0;
        virtual void SetValue(glm::vec2 value, uint32_t offset)  = 0;
        virtual void SetValue(glm::vec3 value, uint32_t offset)  = 0;
        virtual void SetValue(glm::vec4 value, uint32_t offset)  = 0;

        virtual const BufferLayout GetLayout() const = 0;

        static UniformBuffer* Create(const BufferLayout layout, uint32_t bind_point);
    };
}