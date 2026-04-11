#include "Material.h"

#include "Shader.h"
#include "Texture.h"

#include <type_traits>

Material::Material(const Ref<Shader>& shader)
    : m_Shader(shader) {
}

void Material::Bind() const {
    m_Shader->Bind();

    for (const auto& binding : m_Textures) {
        binding.Texture->Bind(binding.Slot);
        m_Shader->SetInt(binding.SamplerName, static_cast<int>(binding.Slot));
    }

    for (const auto& [name, value] : m_Uniforms) {
        ApplyUniform(name, value);
    }
}

void Material::SetInt(const std::string& name, int value) {
    m_Uniforms[name] = value;
}

void Material::SetFloat(const std::string& name, float value) {
    m_Uniforms[name] = value;
}

void Material::SetFloat3(const std::string& name, const Vector3& value) {
    m_Uniforms[name] = value;
}

void Material::SetFloat4(const std::string& name, const Vector4& value) {
    m_Uniforms[name] = value;
}

void Material::SetMat4(const std::string& name, const Matrix4& value) {
    m_Uniforms[name] = value;
}

void Material::SetTexture(const std::string& samplerName, const Ref<Texture2D>& texture, unsigned int slot) {
    for (auto& binding : m_Textures) {
        if (binding.SamplerName == samplerName) {
            binding.Texture = texture;
            binding.Slot = slot;
            return;
        }
    }

    m_Textures.push_back({ samplerName, texture, slot });
}

const Ref<Shader>& Material::GetShader() const {
    return m_Shader;
}

void Material::ApplyUniform(const std::string& name, const UniformValue& value) const {
    std::visit([this, &name](const auto& uniformValue) {
        using ValueType = std::decay_t<decltype(uniformValue)>;

        if constexpr (std::is_same_v<ValueType, int>) {
            m_Shader->SetInt(name, uniformValue);
        } else if constexpr (std::is_same_v<ValueType, float>) {
            m_Shader->SetFloat(name, uniformValue);
        } else if constexpr (std::is_same_v<ValueType, Vector3>) {
            m_Shader->SetFloat3(name, uniformValue);
        } else if constexpr (std::is_same_v<ValueType, Vector4>) {
            m_Shader->SetFloat4(name, uniformValue);
        } else if constexpr (std::is_same_v<ValueType, Matrix4>) {
            m_Shader->SetMat4(name, uniformValue);
        }
    }, value);
}
