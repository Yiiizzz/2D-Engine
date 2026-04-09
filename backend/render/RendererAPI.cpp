#include "RendererAPI.h"

#include "../platform/opengl/OpenGLRendererAPI.h"

#include <stdexcept>

GraphicsAPI RendererAPI::s_API = GraphicsAPI::OpenGL;

Scope<RendererAPI> RendererAPI::Create(GraphicsAPI api) {
    switch (api) {
    case GraphicsAPI::OpenGL:
        return CreateScope<OpenGLRendererAPI>();
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("Requested renderer API is not implemented yet.");
    }
}

GraphicsAPI RendererAPI::GetAPI() {
    return s_API;
}

void RendererAPI::SetAPI(GraphicsAPI api) {
    s_API = api;
}
