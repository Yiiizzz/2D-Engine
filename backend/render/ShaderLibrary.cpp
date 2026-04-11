#include "ShaderLibrary.h"

#include "Shader.h"

#include <stdexcept>

void ShaderLibrary::Add(const Ref<Shader>& shader) {
    Add(shader->GetName(), shader);
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
    m_Shaders[name] = shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {
    const auto shader = Shader::CreateFromFile(filepath);
    Add(shader);
    return shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
    if (Exists(name)) {
        return Get(name);
    }

    const auto shader = Shader::CreateFromFile(name, filepath);
    Add(name, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::Get(const std::string& name) const {
    const auto found = m_Shaders.find(name);
    if (found == m_Shaders.end()) {
        throw std::runtime_error("Shader not found in library: " + name);
    }

    return found->second;
}

bool ShaderLibrary::Exists(const std::string& name) const {
    return m_Shaders.find(name) != m_Shaders.end();
}
