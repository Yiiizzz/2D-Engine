#pragma once

#include "../../render/Shader.h"

#include <string>

class OpenGLShader : public Shader {
public:
    OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
    ~OpenGLShader() override;

    void Bind() const override;
    void Unbind() const override;
    void SetMat4(const std::string& name, const Matrix4& matrix) override;

private:
    unsigned int CompileShader(unsigned int type, const std::string& source);

private:
    unsigned int m_RendererID = 0;
};
