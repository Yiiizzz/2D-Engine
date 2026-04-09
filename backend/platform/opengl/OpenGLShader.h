#pragma once

#include "../../render/Shader.h"

#include <unordered_map>
#include <string>

class OpenGLShader : public Shader {
public:
    OpenGLShader(const std::string& name, const std::string& filepath);
    OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    ~OpenGLShader() override;

    void Bind() const override;
    void Unbind() const override;
    const std::string& GetName() const override;
    void SetInt(const std::string& name, int value) override;
    void SetFloat(const std::string& name, float value) override;
    void SetFloat3(const std::string& name, const Vector3& value) override;
    void SetFloat4(const std::string& name, const Vector4& value) override;
    void SetMat4(const std::string& name, const Matrix4& value) override;

private:
    std::string ReadFile(const std::string& filepath) const;
    std::unordered_map<unsigned int, std::string> PreProcess(const std::string& source) const;
    void Compile(const std::unordered_map<unsigned int, std::string>& shaderSources);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    int GetUniformLocation(const std::string& name);
    void LogShaderError(unsigned int shader, const std::string& stage) const;
    void LogProgramError(unsigned int program) const;

private:
    unsigned int m_RendererID = 0;
    std::string m_Name;
    std::unordered_map<std::string, int> m_UniformLocationCache;
};
