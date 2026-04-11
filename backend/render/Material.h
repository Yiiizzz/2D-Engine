#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "../core/Ref.h"
#include "Math.h"

class Shader;
class Texture2D;

class Material {
public:
    using UniformValue = std::variant<int, float, Vector3, Vector4, Matrix4>;

    explicit Material(const Ref<Shader>& shader);

    void Bind() const;

    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetFloat3(const std::string& name, const Vector3& value);
    void SetFloat4(const std::string& name, const Vector4& value);
    void SetMat4(const std::string& name, const Matrix4& value);
    void SetTexture(const std::string& samplerName, const Ref<Texture2D>& texture, unsigned int slot = 0);

    const Ref<Shader>& GetShader() const;

private:
    void ApplyUniform(const std::string& name, const UniformValue& value) const;

    struct TextureBinding {
        std::string SamplerName;
        Ref<Texture2D> Texture;
        unsigned int Slot = 0;
    };

private:
    Ref<Shader> m_Shader;
    std::unordered_map<std::string, UniformValue> m_Uniforms;
    std::vector<TextureBinding> m_Textures;
};
