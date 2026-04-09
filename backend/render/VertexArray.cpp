#include "VertexArray.h"

#include "../platform/opengl/OpenGLVertexArray.h"
#include "RendererAPI.h"

#include <stdexcept>

std::shared_ptr<VertexArray> VertexArray::Create() {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return std::make_shared<OpenGLVertexArray>();
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("VertexArray backend is not implemented yet.");
    }
}
