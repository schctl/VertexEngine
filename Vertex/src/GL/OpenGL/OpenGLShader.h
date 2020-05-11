#pragma once

#include "Core/Core.h"

#include "GL/Shader.h"

#include "OpenGLUniform.h"

namespace Vertex
{

    // utility function
    const GLenum ShaderDataTypeToOpenGL(ShaderDataType type);

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::vector<unsigned char>& vertex_src, const std::vector<unsigned char>& fragment_src);
        ~OpenGLShader();

        void LoadUniform(const char* uniform_var_name);

        OpenGLUniform& operator[](const char* name);

        void Bind() const override;
        void Unbind() const override;

    private:
        uint32_t          m_ID;
        OpenGLUniformPack m_UniformPack;
    };

}