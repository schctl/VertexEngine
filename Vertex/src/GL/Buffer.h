#pragma once

#include "Core/Core.h"

#include "Shader.h"

namespace Vertex
{
    // -----------------------------------
    // ---------- Buffer Layout ----------
    // -----------------------------------

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