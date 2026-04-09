#include "Shader.h"

#include "../platform/opengl/OpenGLShader.h"

#include <stdexcept>

std::shared_ptr<Shader> Shader::Create(const std::string& vertexSource, const std::string& fragmentSource) {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return std::make_shared<OpenGLShader>(vertexSource, fragmentSource);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("Shader backend is not implemented yet.");
    }
}
