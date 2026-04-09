#include "OpenGLShader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
unsigned int ShaderTypeFromString(const std::string& type) {
    if (type == "vertex") {
        return GL_VERTEX_SHADER;
    }

    if (type == "fragment" || type == "pixel") {
        return GL_FRAGMENT_SHADER;
    }

    throw std::runtime_error("Unknown shader type declaration: " + type);
}
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& filepath)
    : m_Name(name) {
    const std::string source = ReadFile(filepath);
    const auto shaderSources = PreProcess(source);
    Compile(shaderSources);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    : m_Name(name) {
    Compile({
        { GL_VERTEX_SHADER, vertexSource },
        { GL_FRAGMENT_SHADER, fragmentSource }
    });
}

std::string OpenGLShader::ReadFile(const std::string& filepath) const {
    std::ifstream stream(filepath, std::ios::in | std::ios::binary);
    if (!stream) {
        throw std::runtime_error("Failed to open shader file: " + filepath);
    }

    std::ostringstream contents;
    contents << stream.rdbuf();
    return contents.str();
}

std::unordered_map<unsigned int, std::string> OpenGLShader::PreProcess(const std::string& source) const {
    std::unordered_map<unsigned int, std::string> shaderSources;
    static const std::string token = "#type";

    std::size_t pos = source.find(token, 0);
    while (pos != std::string::npos) {
        const std::size_t eol = source.find_first_of("\r\n", pos);
        if (eol == std::string::npos) {
            throw std::runtime_error("Shader file parsing failed: missing line break after #type.");
        }

        const std::size_t begin = pos + token.size() + 1;
        const std::string type = source.substr(begin, eol - begin);
        const unsigned int shaderType = ShaderTypeFromString(type);

        const std::size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        if (nextLinePos == std::string::npos) {
            throw std::runtime_error("Shader file parsing failed: missing shader source after #type.");
        }

        pos = source.find(token, nextLinePos);
        shaderSources[shaderType] = source.substr(nextLinePos, pos == std::string::npos ? source.size() - nextLinePos : pos - nextLinePos);
    }

    if (shaderSources.empty()) {
        throw std::runtime_error("Shader file parsing failed: no shader stages found.");
    }

    return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<unsigned int, std::string>& shaderSources) {
    const auto vertexIt = shaderSources.find(GL_VERTEX_SHADER);
    const auto fragmentIt = shaderSources.find(GL_FRAGMENT_SHADER);
    if (vertexIt == shaderSources.end() || fragmentIt == shaderSources.end()) {
        throw std::runtime_error("Shader compilation failed: vertex and fragment stages are required.");
    }

    const unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexIt->second);
    const unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentIt->second);

    m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, vertexShader);
    glAttachShader(m_RendererID, fragmentShader);
    glLinkProgram(m_RendererID);

    int isLinked = 0;
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        LogProgramError(m_RendererID);
        glDeleteProgram(m_RendererID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw std::runtime_error("OpenGL shader link failed.");
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

const std::string& OpenGLShader::GetName() const {
    return m_Name;
}

void OpenGLShader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void OpenGLShader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void OpenGLShader::SetFloat3(const std::string& name, const Vector3& value) {
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void OpenGLShader::SetFloat4(const std::string& name, const Vector4& value) {
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void OpenGLShader::SetMat4(const std::string& name, const Matrix4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value.Data());
}

unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source) {
    const unsigned int shader = glCreateShader(type);
    const char* sourceCString = source.c_str();
    glShaderSource(shader, 1, &sourceCString, nullptr);
    glCompileShader(shader);

    int isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        LogShaderError(shader, type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
        glDeleteShader(shader);
        throw std::runtime_error("OpenGL shader compile failed.");
    }

    return shader;
}

int OpenGLShader::GetUniformLocation(const std::string& name) {
    const auto found = m_UniformLocationCache.find(name);
    if (found != m_UniformLocationCache.end()) {
        return found->second;
    }

    const int location = glGetUniformLocation(m_RendererID, name.c_str());
    m_UniformLocationCache[name] = location;
    return location;
}

void OpenGLShader::LogShaderError(unsigned int shader, const std::string& stage) const {
    int maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<char> infoLog(static_cast<std::size_t>(maxLength));
    glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());
    std::cerr << stage << " shader compile error:\n" << infoLog.data() << std::endl;
}

void OpenGLShader::LogProgramError(unsigned int program) const {
    int maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<char> infoLog(static_cast<std::size_t>(maxLength));
    glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
    std::cerr << "Shader program link error:\n" << infoLog.data() << std::endl;
}
