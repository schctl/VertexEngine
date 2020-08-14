#pragma once

#include "GL/Buffer.h"

namespace Vertex
{
    // -----------------------------------
    // ---------- Vertex Buffer ----------
    // -----------------------------------

    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(float* vertices, size_t size, const BufferLayout& layout);
        ~OpenGLVertexBuffer();

        void Bind() const override;
        void Unbind() const override;

        inline const BufferLayout& GetLayout() const override { return m_Layout; }

    private:
        uint32_t     m_ID;
        BufferLayout m_Layout;
    };

    // ----------------------------------
    // ---------- Index Buffer ----------
    // ----------------------------------

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t* indices, size_t size);
        ~OpenGLIndexBuffer();

        void Bind() const override;
        void Unbind() const override;

        uint32_t GetCount() const override { return m_Count; }

    private:
        uint32_t m_ID;
        uint32_t m_Count;
    };

    // ----------------------------------
    // --------- Uniform Buffer ---------
    // ----------------------------------

    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(const BufferLayout layout, uint32_t bind_point);
        ~OpenGLUniformBuffer();

        void SetValue(bool value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value);
            Unbind();
        }
        void SetValue(glm::mat3 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0][0]);
            Unbind();
        }
        void SetValue(glm::mat4 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0][0]);
            Unbind();
        }
        void SetValue(int value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value);
            Unbind();
        }
        void SetValue(glm::ivec2 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0]);
            Unbind();
        }
        void SetValue(glm::ivec3 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0]);
            Unbind();
        }
        void SetValue(glm::ivec4 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0]);
            Unbind();
        }
        void SetValue(float value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value);
            Unbind();
        }
        void SetValue(glm::vec2 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0]);
            Unbind();
        }
        void SetValue(glm::vec3 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0]);
            Unbind();
        }
        void SetValue(glm::vec4 value, uint32_t offset) override
        {
            Bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(value), &value[0]);
            Unbind();
        }

        const BufferLayout GetLayout() const { return m_Layout; }

        void Bind() const override;
        void Unbind() const override;

    private:
        uint32_t m_ID;
        uint32_t m_BindPoint;

        const BufferLayout m_Layout;
    };
}