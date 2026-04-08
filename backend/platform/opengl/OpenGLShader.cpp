#include "OpenGLShader.h"

#include <glad/glad.h>

#include <iostream>
#include <stdexcept>
#include <vector>

OpenGLShader::OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource) {
    const unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, vertexShader);
    glAttachShader(m_RendererID, fragmentShader);
    glLinkProgram(m_RendererID);

    int isLinked = 0;
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        int maxLength = 0;
        glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog(static_cast<std::size_t>(maxLength));
        glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, infoLog.data());

        glDeleteProgram(m_RendererID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error("OpenGL shader link failed: " + std::string(infoLog.data()));
    }

    glDetachShader(m_RendererID, vertexShader);
    glDetachShader(m_RendererID, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_RendererID);
}

void OpenGLShader::Bind() const {
    glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const {
    glUseProgram(0);
}

void OpenGLShader::SetMat4(const std::string& name, const Matrix4& matrix) {
    const int location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.Data());
}

unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source) {
    const unsigned int shader = glCreateShader(type);
    const char* sourceCString = source.c_str();
    glShaderSource(shader, 1, &sourceCString, nullptr);
    glCompileShader(shader);

    int isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog(static_cast<std::size_t>(maxLength));
        glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());
        glDeleteShader(shader);

        throw std::runtime_error("OpenGL shader compile failed: " + std::string(infoLog.data()));
    }

    return shader;
}
