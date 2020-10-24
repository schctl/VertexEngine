#include "OpenGLShader.h"

namespace Vertex
{

    const GLenum ShaderDataTypeToOpenGL(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Bool:
            return GL_BOOL;
        case ShaderDataType::Mat3:
            return GL_FLOAT;
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
            return GL_INT;
        case ShaderDataType::Int2:
            return GL_INT;
        case ShaderDataType::Int3:
            return GL_INT;
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Float:
            return GL_FLOAT;
        case ShaderDataType::Float2:
            return GL_FLOAT;
        case ShaderDataType::Float3:
            return GL_FLOAT;
        case ShaderDataType::Float4:
            return GL_FLOAT;
        }

        CoreLogger::Error("Unknown shader data type, cancelling...");
        return 0;
    }

    OpenGLShader::OpenGLShader(std::vector<char>& vertex_src, std::vector<char>& fragment_src) : m_UniformPack(&m_ID)
    {
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        // Load using glShaderBinary
        glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertex_src.data(), vertex_src.size());
        // Specialize the shader (specify the entry point)
        glSpecializeShader(vertex_shader, "main", 0, 0, 0);

        GLint compile_success = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_success);

        if (GL_FALSE == compile_success)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(vertex_shader);

            CoreLogger::Error("{0}", infoLog.data());
            VX_CORE_ASSERT(false, "Vertex Shader compilation failed");
        }

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Load using glShaderBinary
        glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragment_src.data(), fragment_src.size());
        // Specialize the shader (specify the entry point)
        glSpecializeShader(fragment_shader, "main", 0, 0, 0);

        compile_success = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_success);

        if (GL_FALSE == compile_success)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragment_shader);

            CoreLogger::Error("{0}", infoLog.data());
            VX_CORE_ASSERT(false, "Fragment Shader compilation failed");
        }

        // Vertex and fragment shaders are successfully compiled.
        // Link them together into a program and get a program object.
        m_ID = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(m_ID, vertex_shader);
        glAttachShader(m_ID, fragment_shader);

        // Link our program
        glLinkProgram(m_ID);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_ID, GL_LINK_STATUS, (int*)&isLinked);

        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_ID);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            CoreLogger::Error("{0}", infoLog.data());
            VX_CORE_ASSERT(false, "Shader linking failed");
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_ID, vertex_shader);
        glDetachShader(m_ID, fragment_shader);
    }

    OpenGLShader::~OpenGLShader() { glDeleteProgram(m_ID); }

    void OpenGLShader::Bind() const { glUseProgram(m_ID); }

    void OpenGLShader::Unbind() const { glUseProgram(0); }

    OpenGLUniform& OpenGLShader::operator[](const char* name)
    {
        try
        {
            return m_UniformPack[name];
        }
        catch (...)
        {
            m_UniformPack.LoadUniformLocation(name);
            return m_UniformPack[name];
        }
    }

}