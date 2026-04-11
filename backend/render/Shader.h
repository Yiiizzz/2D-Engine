#pragma once

#include <string>

#include "../core/Ref.h"
#include "Math.h"
#include "RendererAPI.h"

class Shader {
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;
    virtual void SetFloat3(const std::string& name, const Vector3& value) = 0;
    virtual void SetFloat4(const std::string& name, const Vector4& value) = 0;
    virtual void SetMat4(const std::string& name, const Matrix4& value) = 0;

    static Ref<Shader> CreateFromFile(const std::string& filepath);
    static Ref<Shader> CreateFromFile(const std::string& name, const std::string& filepath);
    static Ref<Shader> Create(
        const std::string& vertexSource,
        const std::string& fragmentSource,
        const std::string& name = "Shader");
};
