#include "Shader.h"

#include "../platform/opengl/OpenGLShader.h"

#include <filesystem>
#include <stdexcept>

Ref<Shader> Shader::CreateFromFile(const std::string& filepath) {
    const std::filesystem::path path(filepath);
    return CreateFromFile(path.stem().string(), filepath);
}

Ref<Shader> Shader::CreateFromFile(const std::string& name, const std::string& filepath) {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return CreateRef<OpenGLShader>(name, filepath);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("Shader backend is not implemented yet.");
    }
}

Ref<Shader> Shader::Create(
    const std::string& vertexSource,
    const std::string& fragmentSource,
    const std::string& name) {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("Shader backend is not implemented yet.");
    }
}
