#pragma once

#include <memory>
#include <string>

#include "Math.h"
#include "RendererAPI.h"

class Shader {
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void SetMat4(const std::string& name, const Matrix4& matrix) = 0;

    static std::shared_ptr<Shader> Create(const std::string& vertexSource, const std::string& fragmentSource);
};
