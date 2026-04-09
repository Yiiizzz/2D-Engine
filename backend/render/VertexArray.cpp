#include "VertexArray.h"

#include "../platform/opengl/OpenGLVertexArray.h"
#include "RendererAPI.h"

#include <stdexcept>

Ref<VertexArray> VertexArray::Create() {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return CreateRef<OpenGLVertexArray>();
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("VertexArray backend is not implemented yet.");
    }
}
